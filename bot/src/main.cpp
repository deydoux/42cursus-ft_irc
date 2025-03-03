#include "class/Ollama.hpp"

#include <iostream>
#include <string>

int main(void)
{
	Ollama ollama("llama3.2:1b");
	Ollama::context_t context;

	std::string prompt;
	JSON::Object res;

	prompt = "Act like Hello Kitty";
	std::cout << prompt << std::endl << std::endl;

	res = ollama.generate(prompt, context);
	std::cout << res["response"].parse<std::string>() << std::endl << std::endl;

	prompt = "What you written before?";
	std::cout << prompt << std::endl << std::endl;

	res = ollama.generate(prompt, context);
	std::cout << res["response"].parse<std::string>() << std::endl << std::endl;
}
