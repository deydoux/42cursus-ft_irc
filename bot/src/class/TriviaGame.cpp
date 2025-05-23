#include "class/Curl.hpp"
#include "class/IRC.hpp"
#include "class/JSON.hpp"
#include "class/TriviaGame.hpp"

// -- CONSTRUCTOR + DESTRUCTOR

TriviaGame::TriviaGame(IRC &irc_client, const std::string channel_name, std::vector<std::string> players_nicks, bool verbose) :
	_irc_client(irc_client),
	_waiting_for_answers(false),
	_has_ended(false),
	_channel(channel_name),
	_verbose(verbose)
{
	for (size_t i = 0; i < players_nicks.size(); i++) {
		_players.push_back(_create_player(players_nicks[i]));
	}
	log("Creation");
}

TriviaGame::~TriviaGame()
{
	log("Destruction");
}

// -- GETTERS + SETTERS

bool TriviaGame::has_ended( void )
{
	return _has_ended;
}

bool TriviaGame::is_waiting_for_answers( void )
{
	return _waiting_for_answers;
}

bool TriviaGame::is_waiting_before_start( void )
{
	return _waiting_before_start;
}

std::string TriviaGame::get_channel( void )
{
	return _channel;
}

void TriviaGame::set_has_ended(bool has_ended)
{
	_has_ended = has_ended;
}

void TriviaGame::add_player(const std::string &client_nickname)
{
	_players.push_back(_create_player(client_nickname));
}

void TriviaGame::remove_player(const std::string &client_nickname)
{
	player_t *player = _get_player(client_nickname);
	if (!player)
		return ;
	player->is_in_channel = false;

	int active_players_count = 0;
	for (size_t i = 0; i < _players.size(); i++) {
		if (_players[i].is_in_channel)
			active_players_count++;
	}

	if (active_players_count < 2) {
		std::string alert_message = TriviaGame::early_leaving_warning_part1 + _players[0].nickname + TriviaGame::early_leaving_warning_part2;
		_send(alert_message);
		show_final_results();
	}
}

// PUBLIC METHODS

void TriviaGame::tick( void )
{
	if (!_waiting_for_answers)
		return ;
	
	int time_left = calc_time_left(_asked_at, _round_duration_sec);
	if (time_left <= 10 && !_rang_timer) {
		_send(TriviaGame::pick_randomly(TriviaGame::time_warnings));
		_rang_timer = true;
		return ;
	}

	if (time_left <= 0) {
		_send(format("TIMES UP!", BOLD));
		_irc_client.send_raw(_create_empty_reply());
		return _show_round_results();
	}
}

void TriviaGame::greet_players( void )
{
	std::string header = _create_empty_reply();
	header += _create_reply(TriviaGame::greetings_header);
	header += _create_reply(TriviaGame::pick_randomly(TriviaGame::greetings_subheader));
	header += _create_empty_reply();
	_irc_client.send_raw(header, 300);

	std::string rules;
	std::vector<std::string> rules_parts = ft_split(TriviaGame::game_rules, '\n');
	for (size_t i = 0; i < rules_parts.size(); i++) {
		if (rules_parts[i].empty())
			rules += _create_empty_reply();
		else
			rules += _create_reply(rules_parts[i]);
	}
	_irc_client.send_raw(rules, 1000);
	std::string ready = _create_reply(TriviaGame::ask_ready);
	ready += _create_empty_reply();
	_irc_client.send_raw(ready, 1000);

	_waiting_before_start = true;
}

void TriviaGame::show_final_results( void )
{
	_waiting_for_answers = false;

	_send(format("🎉 And that's a wrap! 🎉", BOLD), 500);
	_irc_client.send_raw(_create_empty_reply());
	_send("The trivia showdown has ended, and here are your " + format("final champions:", BOLD), 1000);
	
	std::string player_podium;
	int position = _players.size();
	std::sort(_players.begin(), _players.end(), TriviaGame::_compare_by_total_score);
	for (size_t i = 0; i < _players.size(); i++)
	{
		player_t player = _players[i];
		player_podium = "";

		if (position > 3) player_podium += " ";
		else if (position == 3) player_podium += "🌸";
		else if (position == 2) player_podium += "💖";
		else if (position == 1) player_podium += "🏆";

		player_podium += format(" " + get_ordinal(position) + " place: ", position <= 3 ? BOLD : NO_STYLE);
		player_podium += player.nickname + " - " + to_string(player.total_score) + "pts";

		_send(player_podium, position <= 3 ? 2000 : 1000);

		position--;
	}

	_send(TriviaGame::pick_randomly(TriviaGame::farewells), 1000);
	_has_ended = true;
	return ;
}

void TriviaGame::mark_user_as_ready(const std::string &client_nickname)
{
	player_t *player = _get_player(client_nickname);
	if (!player)
		return ;
	player->is_ready_to_start = true;

	for (size_t i = 0; i < _players.size(); i++) {
		if (!_players[i].is_ready_to_start)
			return ;
	}

	_waiting_before_start = false;
	_start_game();
}

void TriviaGame::store_answer(const std::string &answer, const std::string &client_nickname)
{
	if (answer.size() != 1)
		return ;

	char c_answer = toupper(answer[0]);
	if (_choices.find(c_answer) == _choices.end())
		return ;

	player_t *player = _get_player(client_nickname);
	if (!player || !player->round_answer.empty())
		return ;

	player->round_answer = _choices.find(c_answer)->second;
	
	int score = 0;
	if (player->round_answer == _questions[_round_counter].answer)
	{
		score += _points;
		if (!_first_player_answered) {
			score += _bonus_points;
			player->is_first_to_answer = true;
			_first_player_answered = true;
		}
	}
	player->round_score = score;
	player->total_score += score;

	for (size_t i = 0; i < _players.size(); i++) {
		if (_players[i].round_answer.empty())
			return ;
	}

	_send(TriviaGame::pick_randomly(TriviaGame::teasers_before_results));
	return _show_round_results();
}

void TriviaGame::log(const std::string &message, const log_level level) const
{
	if (_verbose || level != debug)
		::log("Trivia Game", message, level);
}

// -- STATIC METHODS

std::string	TriviaGame::pick_randomly(const phrases_t strings)
{
	if (strings.empty())
		return "";

	size_t random_index = rand() % strings.size();
	return strings[random_index];
}

// -- PRIVATE METHODS

TriviaGame::questions_t TriviaGame::_fetch_questions()
{
	static const std::string url = "https://opentdb.com/api.php?difficulty=easy&amount=" + to_string(_nb_rounds);

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

			question.text = html_decode(question_obj["question"].parse<std::string>());
			question.answer = html_decode(question_obj["correct_answer"].parse<std::string>());
			question.wrong_answers = question_obj["incorrect_answers"].parse_vector<std::string>();
			for (size_t i = 0; i < question.wrong_answers.size(); i++)
				question.wrong_answers[i] = html_decode(question.wrong_answers[i]);
			question.category = html_decode(question_obj["category"].parse<std::string>());
			question.difficulty = html_decode(question_obj["difficulty"].parse<std::string>());

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
	_send("Great! Let me just gather my questions quickly ...");
	_questions = _fetch_questions();

	if (_questions.empty()) {
		_send("Sorry, I couldn't fetch the questions. Let's try again later!");
		_has_ended = true;
		return ;
	}

	_send("Done! Let's not wait another second!");
	_ask_trivia_question();
}

TriviaGame::player_t TriviaGame::_create_player(const std::string nick)
{
	TriviaGame::player_t player;
    player.nickname = nick;
    player.is_in_channel = true;
    player.round_answer = "";
    player.total_score = 0;
    player.is_ready_to_start = false;  
    player.is_first_to_answer = false;
    return player;
}

TriviaGame::player_t *TriviaGame::_get_player(const std::string &nick)
{
	for (size_t i = 0; i < _players.size(); i++) {
		if (_players[i].nickname == nick)
			return &_players[i];
	}
	return NULL;
}

std::string TriviaGame::_create_reply(const std::string &message)
{
	return _irc_client.create_reply("PRIVMSG", _channel, message);
}

std::string TriviaGame::_create_empty_reply( void )
{
	return _irc_client.create_reply("PRIVMSG", _channel, "\t");
}

void TriviaGame::_send(const std::string &message, int send_delay)
{
	_irc_client.send_raw(
		_irc_client.create_reply("PRIVMSG", _channel, message),
		send_delay
	);
}

void TriviaGame::_initialize_round( void )
{
	_waiting_for_answers = true;
	_first_player_answered = false;
	_rang_timer = false;

	player_t *player;
	for (size_t i = 0; i < _players.size(); i++) {
		player = &_players[i];

		player->is_first_to_answer = false;
		player->round_answer = "";
		player->round_score = 0;
	}
}

void TriviaGame::_ask_trivia_question( void )
{
	question_t question = _questions[_round_counter];
	std::string question_raw;

	std::string question_header = "--- 🎯 -- QUESTION ";
	question_header += to_string(_round_counter + 1) + "/" + to_string(_nb_rounds);
	question_header += " -- 🎯 ---";

	question_raw += _create_empty_reply();
	question_raw += _create_reply(format(question_header, BOLD));
	question_raw += _create_empty_reply();
	question_raw += _create_reply(question.text);

	std::vector<std::string> choices;
	choices.push_back(question.answer);
	choices.insert(choices.end(), question.wrong_answers.begin(), question.wrong_answers.end());
	std::random_shuffle(choices.begin(), choices.end());

	_choices.clear();
	std::string alpha = "ABCDEFGHIJ";
	int max_len = get_max_len(choices) + 4;
	for (size_t i = 0; i < choices.size() && i < alpha.size(); i++) {
		_choices[alpha[i]] = choices[i];
		if (i % 2 == 1) {
			question_raw +=  _create_reply(format_inline_choices(
				format_choice(alpha[i - 1], choices[i - 1]),
				format_choice(alpha[i], choices[i]),
				max_len + 4
			));
		} else if (i == choices.size() - 1) {
			question_raw += _create_reply(format_choice(alpha[i], choices[i]));
		}
	}

	question_raw += _create_empty_reply();
	std::string prompt = "⏳ You have " + format(to_string(_round_duration_sec) + " seconds! ", BOLD);
	prompt += TriviaGame::pick_randomly(TriviaGame::question_prompts);
	question_raw += _create_reply(prompt);
	question_raw += _create_empty_reply();

	_irc_client.send_raw(question_raw, 2000);
	_asked_at = time(NULL);

	_initialize_round();
}

void TriviaGame::_show_round_results( void )
{
	_waiting_for_answers = false;

	std::vector<std::string> victorious_players;
	std::string players_answers = "";
	std::string players_scores = "";

	players_answers += _create_reply("🌟 Here are your answers! 🌟");
	for (size_t i = 0; i < _players.size(); i++)
	{
		player_t player = _players[i];
		if (!player.is_in_channel)
			continue ;

		std::string answer = "\t" + player.nickname + ": ";
		answer += player.round_answer.empty() ? format("No response", ITALIC) : player.round_answer;
		players_answers += _create_reply(answer);

		std::string score = "\t" + player.nickname + "  ➝ +" + format(to_string(player.round_score), BOLD);
		if (player.is_first_to_answer)
			score += " (first bonus!) 🎖️";
		else if (player.round_score > 0)
			score += " 🎉";
		players_scores += _create_reply(score);

		if (player.round_score > 0)
			victorious_players.push_back(player.nickname);
	}
	players_answers += _create_empty_reply();
	_irc_client.send_raw(players_answers);

	_send("And the correct answer was...", 2000);
	std::string correct_answer = format(_questions[_round_counter].answer, BOLD);
	_send(correct_answer + "!!!", 1000);

	std::string well_done = _create_empty_reply();
	if (victorious_players.empty()) {
		well_done += _create_reply("Looks like that one was tricky — no winners this round!");
	} else {
		well_done += _create_reply("👏 Well done, " + join_strings(victorious_players) + "!");
		well_done += _create_empty_reply();

		well_done += _create_reply("Here's the score for this round:");
		well_done += players_scores + _create_empty_reply();
	}
	_irc_client.send_raw(well_done, 1000);

	if (_questions.size() <= _round_counter + 1)
		return show_final_results();

	_round_counter++;
	_ask_trivia_question();
}

// -- PRIVATE STATIC METHODS

bool TriviaGame::_compare_by_total_score(const player_t &p1, const player_t &p2)
{
	return p1.total_score < p2.total_score;
}

Curl	TriviaGame::_curl;
