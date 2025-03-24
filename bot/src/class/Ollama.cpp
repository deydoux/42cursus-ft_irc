#include "class/Ollama.hpp"

Ollama::Ollama(const std::string &model) : _model(model) {}

Ollama::Exception::Exception(const std::string &message) : std::runtime_error(message) {}

void Ollama::check()
{
	static const std::string endpoint = _base_uri;
	_curl.get(endpoint);
}

JSON::Object Ollama::generate(const std::string &prompt, context_t &context, const std::string system)
{
	static const std::string endpoint = _base_uri + "/api/generate";\

	JSON::Object data;
	data["model"] = _model;
	data["prompt"] = prompt;
	data["context"] = context;
	data["stream"] = false;
	if (!system.empty())
		data["system"] = system;

	const std::string raw_res = _curl.post(endpoint, data.stringify());
	JSON::Object res = JSON::parse<JSON::Object>(raw_res);

	if (res.find("error") != res.end())
		throw Ollama::Exception(res["error"].parse<std::string>());

	context = res["context"].parse_vector<size_t>();
	return res;
}

Curl Ollama::_curl;
const std::string Ollama::_base_uri = OLLAMA_BASE_URI;
