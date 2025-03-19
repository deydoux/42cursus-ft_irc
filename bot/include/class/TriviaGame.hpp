#ifndef TRIVIAGAME_HPP
#define TRIVIAGAME_HPP

#include "lib.hpp"

#include <unistd.h>

#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <map>

class Curl;
class IRC;

class TriviaGame
{
public:
	// -- STRUCTS
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
		int			round_score;
		int			total_score;
		bool		is_in_channel;
		bool		is_ready_to_start;
		bool		is_first_to_answer;
	} player_t;
	typedef	std::vector<player_t> players_t;

	// -- CONSTRUCTOR + DESTRUCTOR
	TriviaGame(IRC &irc_client, std::string channel_name, std::vector<std::string> players, bool verbose = true);
	~TriviaGame();

	// -- GETTERS + SETTERS
	bool		is_waiting_for_answers( void );
	bool		is_waiting_before_start( void );
	std::string	get_channel( void );

	void		add_player(const std::string &client_nickname);
	void		remove_player(const std::string &client_nickname);

	// -- PUBLIC METHODS
	void	tick( void );
	void	greet_players( void );
	void	show_final_results( void );
	void	mark_user_as_ready(const std::string &client_nickname);
	void	store_answer(const std::string &answer, const std::string &client_nickname);

	void	log(const std::string &message, const log_level level = info) const;

	// -- STATIC METHODS
	static void			initialize_phrases( void );
	static std::string	pick_randomly(const phrases_t phrases);

	// -- I18N
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
	// -- PRIVATE ATTRIBUTES
	IRC					&_irc_client;
	size_t				_round_counter;
	players_t			_players;
	questions_t			_questions;
	std::map<char, std::string> _choices;
	std::time_t			_asked_at;
	bool				_waiting_before_start;
	bool				_waiting_for_answers;
	bool				_first_player_answered;
	bool				_rang_timer;

	// -- PRIVATE CONSTANTS
	const std::string	_channel;
	const bool			_verbose;

	// -- PRIVATE METHODS
	questions_t	_fetch_questions();
	void		_start_game( void );
	player_t	_create_player(const std::string &nick);
	player_t	*_get_player(const std::string &nickname);
	std::string	_create_reply(const std::string &message);
	std::string	_create_empty_reply( void );
	void		_send(const std::string &message, int send_delay = 0);
	void		_initialize_round( void );
	void		_ask_trivia_question( void );
	void		_show_round_results( void );

	// -- PRIVATE STATIC METHODS
	static bool	_compare_by_total_score(const player_t &p1, const player_t &p2);

	// -- PRIVATE STATIC CONSTANTS + ATTRIBUTES
	static const int	_nb_rounds = 10;
	static const int	_round_duration_sec = 30;
	static const int	_points = 10;
	static const int	_bonus_points = 5;

	static Curl	_curl;
};

#endif
