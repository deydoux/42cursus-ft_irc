#include "class/Ollama.hpp"

Ollama::Ollama(const std::string &model) : _model(model) {}

Ollama::Exception::Exception(const std::string &message) : std::runtime_error(message) {}

JSON::Object Ollama::generate(const std::string &prompt, context_t &context)
{
	static const std::string endpoint = _base_uri + "/api/generate";

	JSON::Object data;
	data["model"] = _model;
	data["prompt"] = prompt;
	data["context"] = context;
	data["stream"] = false;

	const std::string raw_res = _curl.post(endpoint, data.stringify());
	JSON::Object res = JSON::parse<JSON::Object>(raw_res);

	if (res.find("error") != res.end())
		throw Ollama::Exception(res["error"].parse<std::string>());

	context_t new_context;
	const JSON::Array res_context = res["context"].parse<JSON::Array>();
	for (JSON::Array::const_iterator it = res_context.begin(); it != res_context.end(); ++it)
		new_context.push_back(it->parse<size_t>());

	context = new_context;
	return res;
}

Curl Ollama::_curl;
const std::string Ollama::_base_uri = OLLAMA_BASE_URI;
