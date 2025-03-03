#include "class/Ollama.hpp"

Ollama::Ollama(const std::string &model) : _model(model) {}

JSON::Object Ollama::generate(const std::string &prompt, context_t &context)
{
	static const std::string endpoint = _base_uri + "/api/generate";

	JSON::Object data;
	data["model"] = _model;
	data["prompt"] = prompt;
	data["stream"] = false;

	const std::string res = _curl.post(endpoint, data.stringify());
	return JSON::parse<JSON::Object>(res);
	(void)context;
}

Curl Ollama::_curl;
const std::string Ollama::_base_uri = OLLAMA_BASE_URI;
