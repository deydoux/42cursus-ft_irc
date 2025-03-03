#include "class/JSON.hpp"

#include <iostream>
#include <string>

int main(void)
{
	JSON::Object obj = JSON::parse<JSON::Object>("            {             \"hello\"             :             \"world\"         }       ");
	std::cout << obj["hello"].parse<std::string>() << std::endl;
}
