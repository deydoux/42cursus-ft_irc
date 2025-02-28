#include "class/JSON.hpp"

#include <iostream>

int main(void)
{
	JSON::Object obj;

	std::vector<int> fib;
	fib.push_back(0);
	fib.push_back(1);
	fib.push_back(1);
	fib.push_back(2);
	fib.push_back(3);
	fib.push_back(5);
	fib.push_back(8);
	fib.push_back(13);
	fib.push_back(21);

	std::cout << obj["text"] << std::endl;
	std::cout << obj["school"] << std::endl;
	std::cout << obj["fib"] << std::endl;

	obj["text"] = "Hello, \" World!";
	obj["school"] = 42;
	obj["fib"] = fib;


	std::cout << obj["text"] << std::endl;
	std::cout << obj["school"] << std::endl;
	std::cout << obj["fib"] << std::endl;

	std::cout << obj << std::endl;
}
