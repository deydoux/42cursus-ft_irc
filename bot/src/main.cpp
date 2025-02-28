#include "class/Ollama.hpp"

#include <iostream>

int main(void)
{
	Ollama ollama("llama3.2:1b");
	Ollama::context_t context;

	std::cout << ollama.generate("Hello Kitty", context) << std::endl;
}
