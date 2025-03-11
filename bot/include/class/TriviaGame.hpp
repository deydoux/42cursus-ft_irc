#ifndef TRIVIAGAME_HPP
#define TRIVIAGAME_HPP

#include "lib.hpp"

#include <cstdlib>
#include <map>

class IRC;

class TriviaGame
{
public:
	typedef	std::vector<std::string> phrases_t;

	TriviaGame(IRC &irc_client, std::string channel_name, std::vector<std::string> players, bool verbose = true);
	~TriviaGame();

	void	log(const std::string &message, const log_level level = info) const;

	bool	is_waiting_for_answers( void );

	void	send(const std::string &message, int send_delay = 0);
	void	add_player(const std::string &client_nickname);
	void	remove_player(const std::string &client_nickname);
	void	greet_players( void );
	void	init_round( void );
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

	static phrases_t	greetings_part1;
	static std::string	greetings_part2;
	static std::string	greetings_part3;
	static std::string	greetings_part4;
	static phrases_t	time_warnings;
	static phrases_t	times_up_warnings;
	static phrases_t	question_prompts;
	static phrases_t	not_enough_players_warnings;
	static phrases_t	room_warnings;
	static phrases_t	farewells;

private:
	IRC					&_irc_client;
	const std::string	_channel;
	std::vector<std::string> _players;
	int					_round_counter;

	bool				_waiting_before_start;
	std::map<std::string, bool> _ready_players;

	const bool			_verbose;

	void	_start_game( void );

	static const int	_nb_rounds = 5;
};

#endif
