#include "class/JSON.hpp"
#include "class/Ollama.hpp"

Ollama::Ollama(const std::string &model) : _model(model) {}

std::string Ollama::generate(const std::string &prompt, context_t &context)
{
	static const std::string endpoint = _base_uri + "/api/generate";

	JSON::Object data;
	data["model"] = _model;
	data["prompt"] = prompt;
	data["stream"] = false;

	return _curl.post(endpoint, data.stringify());
	(void)context;
}

Curl Ollama::_curl;
const std::string Ollama::_base_uri = OLLAMA_BASE_URI;
