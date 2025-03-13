#include "class/Curl.hpp"
#include "class/IRC.hpp"
#include "class/JSON.hpp"
#include "class/TriviaGame.hpp"

#include <unistd.h>

TriviaGame::TriviaGame(IRC &irc_client, const std::string channel_name, std::vector<std::string> players, bool verbose) :
	_irc_client(irc_client),
	_players(players),
	_channel(channel_name),
	_verbose(verbose)
{
	log("Creation");
}

void TriviaGame::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Trivia Game", message, level);
}

TriviaGame::~TriviaGame()
{
	log("Destruction");
}

void TriviaGame::send(const std::string &message, int send_delay)
{
	_irc_client.send_raw(
		_irc_client.create_reply("PRIVMSG", _channel, message),
		send_delay
	);
}

std::string TriviaGame::create_empty_reply( void )
{
	return _irc_client.create_reply("PRIVMSG", _channel, "\t");
}

std::string TriviaGame::create_reply(const std::string &message)
{
	return _irc_client.create_reply("PRIVMSG", _channel, message);
}

void TriviaGame::greet_players( void )
{
	std::string header = create_empty_reply();
	header += create_reply(format(TriviaGame::pick_randomly(TriviaGame::greetings_part1), BOLD));
	_irc_client.send_raw(header, 300);

	std::string rules;
	std::vector<std::string> rules_parts = ft_split(TriviaGame::greetings_part2, '\n');
	for (size_t i = 0; i < rules_parts.size(); i++) {
		rules += create_reply(rules_parts[i]);
	}
	_irc_client.send_raw(rules, 1000);
	send(TriviaGame::greetings_part3, 1000);

	std::string conclusion = create_empty_reply();
	conclusion += create_reply(TriviaGame::greetings_part4);
	_irc_client.send_raw(conclusion, 1000);

	_waiting_before_start = true;
	for (size_t i = 0; i < _players.size(); i++) {
		_ready_players[_players[i]] = false;
	}
}

void TriviaGame::mark_user_as_ready(const std::string &client_nickname)
{
	_ready_players[client_nickname] = true;

	for (size_t i = 0; i < _players.size(); i++) {
		if (!_ready_players[_players[i]])
			return ;
	}

	_waiting_before_start = false;
	_start_game();
}

bool TriviaGame::is_waiting_before_start( void )
{
	return _waiting_before_start;
}

TriviaGame::questions_t TriviaGame::_fetch_questions()
{
	static const std::string url = "https://opentdb.com/api.php?amount=" + to_string(_nb_rounds);

	questions_t questions;

	std::string raw_res;
	try {
		raw_res = _curl.get(url);
	} catch (Curl::Exception &e) {
		log(std::string("Failed to fetch questions: ") + e.what(), error);
		return questions;
	}

	try {
		JSON::Object res = JSON::parse<JSON::Object>(raw_res);

		if (res["response_code"] != 0) {
			sleep(1);
			return _fetch_questions();
		}

		JSON::Array questions_arr = res["results"].parse<JSON::Array>();
		for (JSON::Array::const_iterator it = questions_arr.begin(); it != questions_arr.end(); ++it) {
			question_t question;

			JSON::Object question_obj = it->parse<JSON::Object>();

			question.text = question_obj["question"].parse<std::string>();
			question.answer = question_obj["correct_answer"].parse<std::string>();
			question.wrong_answers = question_obj["incorrect_answers"].parse_vector<std::string>();
			question.category = question_obj["category"].parse<std::string>();
			question.difficulty = question_obj["difficulty"].parse<std::string>();

			questions.push_back(question);
		}

	} catch (JSON::Exception &e) {
		log(std::string("Failed to parse questions: ") + e.what(), error);
	}

	return questions;
}

void TriviaGame::_start_game( void )
{
	_round_counter = 0;
	_questions = _fetch_questions();

	if (_questions.empty()) {
		send("Sorry, I couldn't fetch the questions. Let's try again later!");
		return _irc_client.delete_trivia_game(this);
		//TODO check
	}

	send("Great! Let's start right now!");
	ask_trivia_question();
}

std::string format_choice(char letter, const std::string &choice)
{
	std::string result;
	std::ostringstream oss;

	oss << "[";
	oss << letter;
	oss << "]";

	result = format(oss.str(), BOLD);
	return result + " " + choice;
}

std::string format_inline_choices(const std::string &choice1, const std::string &choice2, int space_between)
{
	std::string result;

	result = choice1 + std::string(std::max(space_between, 20) - choice1.size(), ' ');
	return result + choice2;
}

int get_max_len(std::vector<std::string> arr)
{
	size_t max_len = 0;
	for (size_t i = 0; i < arr.size(); ++i) {
		if (arr[i].size() > max_len)
			max_len = arr[i].size();
	}

	return max_len;
}

void TriviaGame::ask_trivia_question( void )
{
	question_t question = _questions[_round_counter];
	std::string question_raw;

	question_raw += create_empty_reply();
	question_raw += create_reply(format("--- 🎯 -- QUESTION 1/" + to_string(_nb_rounds) + " -- 🎯 ---", BOLD));
	question_raw += create_empty_reply();
	question_raw += create_reply(question.text);

	std::vector<std::string> choices;
	choices.push_back(question.answer);
	choices.insert(choices.end(), question.wrong_answers.begin(), question.wrong_answers.end());
	std::random_shuffle(choices.begin(), choices.end());

	std::string alpha = "ABCDEFGHIJ";
	int max_len = get_max_len(choices) + 4;
	for (size_t i = 0; i < choices.size() && i < alpha.size(); i++) {
		_choices[alpha[i]] = choices[i];
		if (i % 2 == 1) {
			question_raw +=  create_reply(format_inline_choices(
				format_choice(alpha[i - 1], choices[i - 1]),
				format_choice(alpha[i], choices[i]),
				max_len + 4
			));
		} else if (i == choices.size() - 1) {
			question_raw += create_reply(format_choice(alpha[i], choices[i]));
		}
	}

	question_raw += create_empty_reply();
	std::string prompt = "⏳ You have " + format(to_string(_round_duration_sec) + " seconds! ", BOLD);
	prompt += TriviaGame::pick_randomly(TriviaGame::question_prompts);
	question_raw += create_reply(prompt);
	question_raw += create_empty_reply();

	_irc_client.send_raw(question_raw, 500);
	_asked_at = time(NULL);
	_waiting_for_answers = true;
}

void TriviaGame::store_answer(const std::string &answer, const std::string &client_nickname)
{
	(void)answer;
	(void)client_nickname;
	// This function is called every time someones sends a privatemessage to the channel in which the game is going on
	// AND that the game is waiting for an answer from the players.

	// If the player hasn't already answered the question, it stores his answer in the answers vector (does nothing otherwise)
	// It also updates his score accordingly
	// --> if the player is the first to answer the question and his answer is correct,
	//	   then it adds 10 points (static const) to its original score

	// If there's no more player to wait for in the channel, simply call show_round_results()
	// (which should put an end to the round)
}

void TriviaGame::tick( void )
{
	// tick() is supposed to be a function called repeatedly in the main loop of the bot, for every ongoing trivia game.
	// It must check first if the game is waiting for an answer of the players (so that a question was just asked)
	// -> if not, then their's nothing to be done here (return)

	// It checks the game's timer (game starting time) and sends a message if their's only 10 seconds left:
	/*
		5 seconds left! Panic! (Just kidding. Or am I?) ⏳😼
		(Here are some alternative messages for this part, which also needs to be picked randomly)
		- Tick-tock! Your brain better be in turbo mode now! 🏎️💨
		- 5 seconds! Choose wisely, or forever hold your regret. 😼
		- If you're still thinking, think FASTER! 🧠⚡
		- HELLO?? Time is running out! Pick something! 🏃‍♀️💨
		- 5 seconds left! Do you believe in miracles? 😹
		- Quick! Eeny, meeny, miny... just PICK ONE! 🎲
		- If you haven’t answered yet, now’s a good time to start screaming. 🔥
		- Almost out of time! Make a choice before I start judging... 👀
		- 5 seconds! Do you trust your gut, or is it full of doubt? 🤔
	*/

	// If the timer's out and the bot is still waiting for a player to answer, then it uses show_round_results()
	// (which should put an end to the round)
}

void TriviaGame::show_round_results( void )
{
	// It stops waiting for answers, and show the results of the round in this format :
	// (again, it can be nice to send this chunk by chunk with a small delay between each one of them)
	/*
		[Times up!] chunk 0
		[🌟 Here are your answers! 🌟

		- quteriss: Star City
		- mapale: Metropolis
		- deydoux: Star City] chunk 1

		[And the correct answer was...] chunk 2
		[**Star City!!!** 🏹✨] chunk 3

		[👏 Well done, quteriss and deydoux!

		Here’s the score for this round:
		quteriss  ➝ **+15** (first bonus!) 🎖️
		deydoux  ➝ **+10** 🎉
		mapale  ➝ **+0** ] chunk 4
	*/

	// If the round counters is the max number of rounds for this Trivia Game,
	// then it calls show_final_results() and returns immediatly

	// Otherwise, it calls init_round(), and then ask_trivia_question();
}

void TriviaGame::show_final_results( void )
{
	// it must stop the waiting_for_answers
	// Displays the final results of the game
	/*
		🎉 **And that’s a wrap!** 🎉

		The trivia showdown has ended, and here are your **final champions**:

		[🌸 **3rd Place**: deydoux - **30**pts] chunk 1
		[🎀 **2nd Place**: quteriss - **45**pts] chunk 2
		[💖 **1st Place**: mapale - **135**pts] chunk 3 ...

		👏 Well done to everyone who played! It’s been a blast—whether you were a trivia genius or just here for the fun! 😸
	*/

	// Then it calls the irc_client delete_trivia_game(this) function, that destroys the current TriviaGame instance
}

bool TriviaGame::is_waiting_for_answers( void )
{
	return _waiting_for_answers;
}

std::string TriviaGame::get_channel( void )
{
	return _channel;
}

void TriviaGame::add_player(const std::string &client_nickname)
{
	(void)client_nickname;
	// Adds a player to the player list
	// --> this action should not interrupt the game
}

void TriviaGame::remove_player(const std::string &client_nickname)
{
	(void)client_nickname;
	// Removes the player from the player list
	// --> preferably, this action should not remove the actual score of the quitting client
	//	   so that, in the final scores, he still appears (even if he/she has a disadvantage)
	// Check if there's still enough player to play, and if not:
		// - send a room_warnings message (pick randomly)
		// - show the final results
}

std::string	TriviaGame::pick_randomly(const phrases_t strings)
{
	if (strings.empty())
		return "";

	size_t random_index = rand() % strings.size();
	return strings[random_index];
}

Curl	TriviaGame::_curl;
