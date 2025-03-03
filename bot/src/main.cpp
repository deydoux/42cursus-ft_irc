#include "class/Ollama.hpp"

#include <iostream>
#include <string>

int main(void)
{
	Ollama ollama("llama3.2:1b");
	Ollama::context_t context;

	JSON::Object res = ollama.generate("Act like Hello Kitty", context);
	// std::cout << res << std::endl;
	std::cout << res["response"].parse<std::string>() << std::endl;
}
