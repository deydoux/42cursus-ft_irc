#include "class/JSON.hpp"

#include <iostream>

int main(void)
{
	std::cout << JSON::parse<JSON::Object>("{\"hello\":\"world\"}") << std::endl;
}
