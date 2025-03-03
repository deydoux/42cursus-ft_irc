#include "class/JSON.hpp"

#include <iostream>
#include <string>

int main(void)
{
	JSON::Object obj = JSON::parse<JSON::Object>("   {   \"hello\"   :   \"world\"   ,   \"school\"   :   42   ,   \"truth\"   :   true   ,   \"cat\"   :   { \"height\" : 5 , \"color\" : \"orange\" }   }   ");
	JSON::Object cat = obj["cat"].parse<JSON::Object>();

	std::cout << "obj.hello = " << obj["hello"] << std::endl;
	std::cout << "obj.school = " << obj["school"] << std::endl;
	std::cout << "obj.truth = " << obj["truth"] << std::endl;
	std::cout << "obj.cat = " << cat << std::endl;
	std::cout << "obj.cat.height = " << cat["height"] << std::endl;
	std::cout << "obj.cat.color = " << cat["color"] << std::endl;
}
