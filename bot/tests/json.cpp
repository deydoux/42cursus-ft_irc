#include "class/JSON.hpp"

#include <iostream>
#include <string>

int main(void)
{
	JSON::Object obj = JSON::parse<JSON::Object>("   {   \"hello\"   :   \"world\"   ,   \"school\"   :   42   ,   \"truth\"   :   true   ,   \"cat\"   :   { \"height\" : 5 , \"color\" : \"orange\" },\"empty\":null, \"fib\": [0, 1, 1, 2, 3, 5, 8, 13, 21]}   ");

	std::cout << "obj = " << obj << std::endl;
	std::cout << "obj.hello = " << obj["hello"] << std::endl;
	std::cout << "obj.school = " << obj["school"] << std::endl;
	std::cout << "obj.truth = " << obj["truth"] << std::endl;

	JSON::Object cat = obj["cat"].parse<JSON::Object>();
	std::cout << "obj.cat = " << cat << std::endl;
	std::cout << "obj.cat.height = " << cat["height"] << std::endl;
	std::cout << "obj.cat.color = " << cat["color"] << std::endl;

	std::cout << "obj.empty = " << obj["empty"] << std::endl;

	JSON::Array fib = obj["fib"].parse<JSON::Array>();
	std::cout << "obj.fib = " << fib << std::endl;
	for (size_t i = 0; i < fib.size(); ++i) {
		std::cout << "obj.fib[" << i << "] = " << fib[i].parse<size_t>() << std::endl;
	}
}
