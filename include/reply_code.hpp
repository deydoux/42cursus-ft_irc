#ifndef REPLY_CODE_HPP
#define REPLY_CODE_HPP

typedef enum {
	RPL_WELCOME = 1,
	RPL_YOURHOST = 2,
	RPL_CREATED = 3,
	RPL_MYINFO = 4,
	RPL_ISUPPORT = 5,
	RPL_STATSDLINE = 250,
	RPL_LUSERCLIENT = 251,
	RPL_LUSERCHANNELS = 254,
	RPL_LUSERME = 255,
	RPL_LOCALUSERS = 265,
	RPL_GLOBALUSERS = 266,
	RPL_ENDOFWHO = 315,
	RPL_CHANNELMODEIS = 324,
	RPL_CREATIONTIME = 329,
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_TOPICWHOTIME = 333,
	RPL_INVITING = 341,
	RPL_WHOREPLY = 352,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	RPL_MOTD = 372,
	RPL_MOTDSTART = 375,
	RPL_ENDOFMOTD = 376,
	RPL_HK = 377,
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHSERVER = 402,
	ERR_NOSUCHCHANNEL = 403,
	ERR_TOOMANYCHANNELS = 405,
	ERR_NORECIPIENT = 411,
	ERR_NOTEXTTOSEND = 412,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NOMOTD = 422,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_CHANNELISFULL = 471,
	ERR_UNKNOWNMODE = 472,
	ERR_INVITEONLYCHAN = 473,
	ERR_BANNEDFROMCHAN = 474,
	ERR_BADCHANNELKEY = 475,
	ERR_CHANOPRIVSNEEDED = 482,
	ERR_USERSDONTMATCH = 502
} reply_code;

#endif // REPLY_CODE_HPP
