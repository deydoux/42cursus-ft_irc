//
#include "class/Curl.hpp"
#include "class/JSON.hpp"

#include <iostream>
#include <string>

int main()
{
	Curl curl;

	const std::string raw_res = curl.get("https://opentdb.com/api.php?amount=10");
	std::cout << raw_res << std::endl << std::endl;

	JSON::Object res = JSON::parse<JSON::Object>(raw_res);
	std::cout << res << std::endl << std::endl;

	JSON::Array results = res["results"].parse<JSON::Array>();
	std::cout << results << std::endl << std::endl;

	for (JSON::Array::const_iterator it = results.begin(); it != results.end(); ++it) {
		JSON::Object obj = it->parse<JSON::Object>();

		std::string difficulty = obj["difficulty"].parse<std::string>();
		std::string category = obj["category"].parse<std::string>();
		std::string question = obj["question"].parse<std::string>();
		std::string correct_answer = obj["correct_answer"].parse<std::string>();
		JSON::Array incorrect_answers = obj["incorrect_answers"].parse<JSON::Array>();

		std::cout << "Category: " << category << std::endl;
		std::cout << "Difficulty: " << difficulty << std::endl;
		std::cout << "Question: " << question << std::endl;
		std::cout << "Correct answer: " << correct_answer << std::endl;
		std::cout << "Incorrect answers: " << incorrect_answers << std::endl << std::endl;
	}
}
