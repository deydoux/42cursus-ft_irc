#ifndef OLLAMA_HPP
#define OLLAMA_HPP

#include "class/Curl.hpp"

#include <string>
#include <vector>

class Ollama
{
public:
	Ollama(const std::string &model);

	std::string	generate(const std::string &prompt);
private:
	std::string	_model;
	Curl		_curl;
};

#endif
