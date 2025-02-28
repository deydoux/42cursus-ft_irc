#include "class/JSON.hpp"

#include <iostream>

int main(void)
{
	std::cout << JSON::parse<int>("42") << std::endl;
}
