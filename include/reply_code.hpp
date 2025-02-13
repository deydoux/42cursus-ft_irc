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
	RPL_MOTD = 372,
	RPL_MOTDSTART = 375,
	RPL_ENDOFMOTD = 376,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NOMOTD = 422,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTRED = 462
} reply_code;

#endif // REPLY_CODE_HPP
