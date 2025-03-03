#include "class/JSON.hpp"

#include <iostream>
#include <string>

int main(void)
{
	JSON::Object obj = JSON::parse<JSON::Object>("   {   \"hello\"   :   \"world\"   ,   \"school\"   :   42   ,   \"truth\"   :   true   ,   \"cat\"   :   { \"height\" : 5 , \"color\" : \"orange\" },\"empty\":null, \"fib\": [0, 1, 1, 2, 3, 5, 8, 13, 21]}   ");
	JSON::Object cat = obj["cat"].parse<JSON::Object>();

	std::cout << "obj = " << obj << std::endl;
	std::cout << "obj.hello = " << obj["hello"] << std::endl;
	std::cout << "obj.school = " << obj["school"] << std::endl;
	std::cout << "obj.truth = " << obj["truth"] << std::endl;
	std::cout << "obj.cat = " << cat << std::endl;
	std::cout << "obj.cat.height = " << cat["height"] << std::endl;
	std::cout << "obj.cat.color = " << cat["color"] << std::endl;
	std::cout << "obj.empty = " << obj["empty"] << std::endl;
	std::cout << "obj.fib = " << obj["fib"] << std::endl;
}
