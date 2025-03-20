#ifndef OLLAMA_HPP
#define OLLAMA_HPP

#include "class/Curl.hpp"
#include "class/JSON.hpp"

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

	class Exception : public std::runtime_error
	{
	public:
		Exception(const std::string &message);
	};

	void	check();

	JSON::Object	generate(const std::string &prompt, context_t &context, const std::string system = "");

private:
	const std::string	_model;

	static Curl					_curl;
	static const std::string	_base_uri;
};

#endif
