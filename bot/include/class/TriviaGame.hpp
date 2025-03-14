#ifndef TRIVIAGAME_HPP
#define TRIVIAGAME_HPP

#include "lib.hpp"

#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <map>

class Curl;
class IRC;

class TriviaGame
{
public:
	typedef	std::vector<std::string> phrases_t;
	
	typedef	struct question_s {
		std::string					text;
		std::string					answer;
		std::vector<std::string>	wrong_answers;
		std::string					category;
		std::string					difficulty;
	}	question_t;
	typedef	std::vector<question_t> questions_t;

	typedef struct player_s {
		std::string	nickname;
		std::string	round_answer;
		std::string	total_score;
		bool		is_in_channel;
		bool		is_ready_to_start;
	} player_t;
	typedef	std::vector<player_t> players_t;

	TriviaGame(IRC &irc_client, std::string channel_name, std::vector<std::string> players, bool verbose = true);
	~TriviaGame();

	void	log(const std::string &message, const log_level level = info) const;

	bool	is_waiting_for_answers( void );

	void	send(const std::string &message, int send_delay = 0);
	std::string create_reply(const std::string &message);
	std::string create_empty_reply( void );
	void	add_player(const std::string &client_nickname);
	void	remove_player(const std::string &client_nickname);
	void	greet_players( void );
	void	tick( void );
	void	ask_trivia_question( void );
	void	store_answer(const std::string &answer, const std::string &client_nickname);
	void	show_round_results( void );
	void	show_final_results( void );
	void	mark_user_as_ready(const std::string &client_nickname);
	bool	is_waiting_before_start( void );

	std::string get_channel( void);

	static void			initialize_phrases( void );
	static std::string	pick_randomly(const phrases_t phrases);

	player_t	create_player(const std::string &nick);

	static std::string	greetings_header;
	static phrases_t	greetings_subheader;
	static std::string	game_rules;
	static std::string	ask_ready;

	static phrases_t	time_warnings;
	static phrases_t	times_up_warnings;
	static phrases_t	question_prompts;
	static phrases_t	not_enough_players_warnings;
	static phrases_t	teasers_before_results;
	static std::string	early_leaving_warning_part1;
	static std::string	early_leaving_warning_part2;
	static phrases_t	farewells;

private:
	IRC					&_irc_client;
	players_t			_players;
	int					_round_counter;
	questions_t			_questions;
	std::map<char, std::string> _choices;
	std::time_t			_asked_at;
	bool				_waiting_before_start;
	bool				_waiting_for_answers;
	bool				_first_player_answered;

	bool				_rang_timer;

	const std::string	_channel;
	const bool			_verbose;

	questions_t	_fetch_questions();
	void		_start_game( void );
	player_t	*_get_player(const std::string &nickname);

	static const int	_nb_rounds = 5;
	static const int	_round_duration_sec = 50;
	static const int	_points = 10;
	static const int	_bonus_points = 2;

	static Curl	_curl;
};

#endif
