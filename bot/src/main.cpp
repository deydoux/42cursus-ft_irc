#include "class/JSON.hpp"

#include <iostream>

int main(void)
{
	JSON::Object obj = JSON::parse<JSON::Object>("{\"hello\":\"world\"}");
	std::cout << obj["hello"] << std::endl;
}
