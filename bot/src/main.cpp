#include "class/Curl.hpp"

#include <iostream>

int main(void)
{
	Curl curl;

	std::cout << curl.post("http://localhost:11434/api/generate", "{\"model\":\"llama3.2:1b\",\"prompt\":\"Shortly brief what's Hello Kitty\",\"stream\":false}") << std::endl;
}
