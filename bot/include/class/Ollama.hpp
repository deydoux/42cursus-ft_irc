#ifndef OLLAMA_HPP
#define OLLAMA_HPP

#include "class/Curl.hpp"

#include <string>
#include <vector>

#ifndef OLLAMA_BASE_URI
# define OLLAMA_BASE_URI "http://localhost:11434"
#endif

class Ollama
{
public:
	typedef std::vector<size_t>	context_t;

	Ollama(const std::string &model);

	std::string	generate(const std::string &prompt, context_t &context);

private:
	const std::string	_model;

	static Curl					_curl;
	static const std::string	_base_uri;
};

#endif
