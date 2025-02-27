#include "class/Curl.hpp"

#include <iostream>

int main(void)
{
	Curl curl;

	std::cout << curl.get("https://neofetch.dorian.cool") << std::endl;
}
