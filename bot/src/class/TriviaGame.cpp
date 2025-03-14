#include "class/TriviaGame.hpp"
#include "class/IRC.hpp"

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
	header += create_reply(TriviaGame::greetings_header);
	header += create_reply(TriviaGame::pick_randomly(TriviaGame::greetings_subheader));
	header += create_empty_reply();
	_irc_client.send_raw(header, 300);
	
	std::string rules;
	std::vector<std::string> rules_parts = ft_split(TriviaGame::game_rules, '\n');
	for (size_t i = 0; i < rules_parts.size(); i++) {
		if (rules_parts[i].empty())
			rules += create_empty_reply();
		else
			rules += create_reply(rules_parts[i]);
	}
	_irc_client.send_raw(rules, 1000);
	std::string ready = create_reply(TriviaGame::ask_ready);
	ready += create_empty_reply();
	_irc_client.send_raw(ready, 1000);

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

void TriviaGame::_start_game( void )
{
	_round_counter = 0;

	send("Great! Let's start right now!");

	// It musts fetch the questions using the Curl object and the Trivia Game API (https://opentdb.com/api_config.php),
	// with a api url looking something like this: https://opentdb.com/api.php?amount=1
	// Then it parses the possible answers using the JSON object, and stores the answer
	// (the users answers will not be checked in this function, so this is necessary)
	
	_questions.push_back((question_t) {
		.text = "What is the capital of Australia?",
		.answer = "Canberra",
		.wrong_answers = std::vector<std::string>(3, "")
	});
	_questions.back().wrong_answers[0] = "Sydney";
	_questions.back().wrong_answers[1] = "Melbourne";
	_questions.back().wrong_answers[2] = "Brisbane";

	_questions.push_back((question_t) {
		.text = "Who wrote 'To Kill a Mockingbird'?",
		.answer = "Harper Lee",
		.wrong_answers = std::vector<std::string>(3, "")
	});
	_questions.back().wrong_answers[0] = "Mark Twain";
	_questions.back().wrong_answers[1] = "J.D. Salinger";
	_questions.back().wrong_answers[2] = "Ernest Hemingway";

	_questions.push_back((question_t) {
		.text = "Which element has the chemical symbol 'O'?",
		.answer = "Oxygen",
		.wrong_answers = std::vector<std::string>(3, "")
	});
	_questions.back().wrong_answers[0] = "Osmium";
	_questions.back().wrong_answers[1] = "Gold";
	_questions.back().wrong_answers[2] = "Silver";

	std::random_shuffle(_questions.begin(), _questions.end());

	ask_trivia_question();
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
	_rang_timer = false;
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
	if (!_waiting_for_answers)
		return ;
	
	int time_left = calc_time_left(_asked_at, _round_duration_sec);
	if (time_left <= 10 && !_rang_timer) {
		send(TriviaGame::pick_randomly(TriviaGame::time_warnings));
		_rang_timer = true;
		return ;
	}

	if (time_left <= 0) {
		send(format("TIMES UP!", BOLD));
		return show_round_results();
	}
}

void TriviaGame::show_round_results( void )
{
	_waiting_for_answers = false;
	
	/*
	std::vector<std::string> victorious_players;
	std::string players_answers;
	std::string players_scores;

	players_answers += create_reply("🌟 Here are your answers! 🌟");
	for (size_t i = 0; i < _players.size(); i++)
	{
		player_t player = _players[i];
		if (!player.is_in_channel)
			continue ;

		std::string answer = "\t" + player.nickname + ": ";
		answer += it.round_answer.empty() ? format("no response", ITALIC) : it.round_answer;
		players_answers += create_reply(answer);

		std::string score = "\t" + player.nickname + "  ➝ +" + format(to_string(player.round_points), BOLD);
		if (player.is_first_to_answer)
			score += " (first bonus!) 🎖️";
		else if (player.round_points > 0)
			score += " 🎉";
		players_scores += create_reply(score);

		if (player.round_points > 0)
			victorious_players.push_back(player.nickname);
	}
	players_answers += create_empty_reply();
	_irc_client.send_raw(players_answers);

	send("And the correct answer was...", 2000);
	std::string correct_answer = format(_questions[_round_counter].answer, BOLD);
	send(correct_answer + "!!!", 1000);

	std::string well_done = create_empty_reply();
	if (victorious_players.empty())
		well_done += create_reply("Looks like that one was tricky — no winners this round!");
	else
		well_done += create_reply("👏 Well done, " + join_strings(victorious_players) + "!");
	well_done += create_empty_reply();
	_irc_client.send_raw(well_done, 1000);

	std::string scores = send("Here’s the score for this round:", 1000);
	players_scores += create_empty_reply();
	_irc_client.send_raw(players_scores);
	*/

	if (_questions.size() <= _round_counter + 1)
		return show_final_results();

	ask_trivia_question();
}

//bool TriviaGame::compare_by_total_score(const player_t &p1, const player_t &p2)
//{
//	return p1.total_score < p2.total_score;
//}


void TriviaGame::show_final_results( void )
{
	_waiting_for_answers = false;

	/*
	send(format("🎉 And that’s a wrap! 🎉", BOLD), 500);
	send("The trivia showdown has ended, and here are your " + format("final champions:", BOLD), 1000);
	
	std::string player_podium;
	int position = _players.size();
	std::sort(_players.begin(), _players.end(), compare_by_total_score);
	for (size_t i = 0; i < _players.size(); i++)
	{
		player_t player = _players[i];
		player_podium = "";

		if (position > 3) player_podium += " ";
		else if (position == 3) player_podium += "🌸";
		else if (position == 2) player_podium += "💖";
		else if (position == 1) player_podium += "🏆";

		player_podium += format(" " + get_ordinal(position) + " place: ", positon > 3 ? BOLD : NO_STYLE);
		player_podium += player.nickname + " - " + player.total_score + "pts";

		send(player_podium, position > 3 ? 1000 : 500);

		position--;
	}

	send(TriviaGame::pick_randomly(TriviaGame::farewells), 1000);
	return ;
	*/
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
	_players.push_back(client_nickname);
	_players_scores[client_nickname] = 0;
}

void TriviaGame::remove_player(const std::string &client_nickname)
{
	_players.erase(std::remove(_players.begin(), _players.end(), client_nickname), _players.end());
	
	if (_players.size() < 2) {
		std::string alert_message = TriviaGame::early_leaving_warning_part1 + _players[0] + TriviaGame::early_leaving_warning_part2;
		send(alert_message);
		show_final_results();
	}
}

std::string	TriviaGame::pick_randomly(const phrases_t strings)
{
	if (strings.empty())
		return "";

	size_t random_index = rand() % strings.size();
	return strings[random_index];
}
