#include "class/TriviaGame.hpp"
#include "class/IRC.hpp"

TriviaGame::TriviaGame(IRC &irc_client, const std::string channel_name, std::vector<std::string> players, bool verbose) :
	_irc_client(irc_client),
	_channel(channel_name),
	_players(players),
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

void TriviaGame::greet_players( void )
{
	send(TriviaGame::pick_randomly(TriviaGame::greetings_part1), 500);
	
	std::string rules;
	std::vector<std::string> rules_parts = ft_split(TriviaGame::greetings_part2, '\n');
	for (size_t i = 0; i < rules_parts.size(); i++) {
		rules += _irc_client.create_reply("PRIVMSG", _channel, rules_parts[i]);
	}
	_irc_client.send_raw(rules, 1000);

	send(TriviaGame::greetings_part3, 1000);
	send(TriviaGame::greetings_part4, 1000);

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
	// Starts the round counter, and uses init_round() to prepare the coming round.
	_round_counter = 0;

	log("Starting the game!", info);

	// Fetch all the questions for the trivia and store them

	// Then, using ask_trivia_question(), it asks the first question to the users.
	ask_trivia_question();
}

void TriviaGame::init_round( void )
{
	// increments the round counter by 1
	// initializes every futur data necessary for the round
		// - the correct answer
		// - the player answers
		// - first player to answer
		// - the player scores
		// - the random seed (srand(time(NULL)))
		// ...
}

void TriviaGame::ask_trivia_question( void )
{
	// It musts fetch the question using the Curl object and the Trivia Game API (https://opentdb.com/api_config.php),
	// with a api url looking something like this: https://opentdb.com/api.php?amount=1

	// Then it parses the possible answers using the JSON object, and stores the answer
	// (the users answers will not be checked in this function, so this is necessary)

	// Sends a message with the question to users. Sending this chunk by chunk isn't necessary
	/*
		🎯 **Question 1/5** 🎯  

		In DC Comics, where does the Green Arrow (Oliver Queen) live?  

		[A] Star City    	[C] Metropolis
		[B] Gotham City  	[D] Central City   

		⏳ You have **30 seconds**! So, what's your guess? 🤔
	*/

	// Here are a few alternative messages to "So what's your guess ? 🤔"
	// Something cool would be that it selects one randomly each time, picking from a static list (should it be local or global?)
	// => this means that emojies must be sent correctly!
	/*
		- Lock in your answer! 🏹
		- Take your best shot! 🎯
		- Think fast! What’s your pick? ⏳
		- Make your choice, hero! 🦸
		- Time’s ticking—what’s your answer? ⏰
		- Go with your gut! What’ll it be? 🤔
		- Choose wisely! 🧠✨
		- What’s your final answer? 🔥
		- Trust your instincts—what do you say? 🧐
		- Don't overthink it! Pick one! 🚀
	*/

	// It starts the timer (simply store the time when the question was asked, 
	// it will then be compared with the current time in the tick() function)
	// --> the time needed to complete a round needs to be stored in a static variable, 
	//     so we can change it later for a better experience
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
	// Simply returns the value of a private variable _is_waiting_for_answers,
	// which is supposed to be true if a question was asked and that the trivia bot is waiting for the players to answer it
	return false;
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
