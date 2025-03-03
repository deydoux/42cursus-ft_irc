#include "class/Ollama.hpp"

#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
	Ollama ollama("llama3.2:1b");
	Ollama::context_t context;

	if (argc > 1) {
		JSON::Object res = ollama.generate(argv[1], context);
		std::cout << res["response"].parse<std::string>() << std::endl;

		return 0;
	}

	while (!std::cin.eof()) {
		std::cout << "> ";

		std::string prompt;
		std::getline(std::cin, prompt);

		if (prompt.empty())
			continue;

		JSON::Object res = ollama.generate(prompt, context);
		std::cout << std::endl << res["response"].parse<std::string>() << std::endl << std::endl;
	}
}
