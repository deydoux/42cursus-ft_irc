#include "class/JSON.hpp"

#include <iostream>

int main(void)
{
	std::cout << JSON::parse<std::string>("42") << std::endl;
}
