#ifndef TRIVIAGAME_HPP
#define TRIVIAGAME_HPP

#include "lib.hpp"

#include <cstdlib>

class IRC;

class TriviaGame
{
public:
	TriviaGame(IRC &irc_client, std::string channel_name, std::vector<std::string> players, bool verbose = true);
	~TriviaGame();

	void	log(const std::string &message, const log_level level = info) const;

	bool	is_waiting_for_answers( void );

	void	send(const std::string &message);
	void	add_player(const std::string &client_nickname);
	void	remove_player(const std::string &client_nickname);
	void	greet_players( void );
	void	init_round( void );
	void	tick( void );
	void	ask_trivia_question( void );
	void	store_answer(const std::string &answer, const std::string &client_nickname);
	void	show_round_results( void );
	void	show_final_results( void );

	std::string get_channel( void);

	static std::string	pick_randomly(const std::vector<std::string> phrases);

private:
	IRC					&_irc_client;
	const std::string	_channel;
	std::vector<std::string> _players;

	const bool			_verbose;

	static const int	_nb_rounds = 5;
};

#endif
