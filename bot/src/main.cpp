#include "class/Ollama.hpp"
#include "class/JSON.hpp"

#include <iostream>

int main(void)
{
	JSON::SimpleObject obj;
	obj["key"] = "value";
	std::cout << obj.stringify() << std::endl;

	Ollama ollama("llama3.2:1b");
	Ollama::context_t context;

	std::cout << ollama.generate("Bonjour", context) << std::endl;
}
