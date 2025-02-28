#ifndef JSON_HPP
#define JSON_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

class JSON
{
	public:
	class Value
	{
	public:
		Value();
		Value(const Value &other);
		template <typename T>
		Value(const T &value);

		Value	&operator=(const Value &rhs);
		template <typename T>
		Value	&operator=(const T &rhs);

		std::string	stringify() const;

	private:
		std::string _str;
	};

	class Array : public std::vector<Value>
	{
	public:
		std::string	stringify() const;
	};

	class Object : public std::map<std::string, Value>
	{
	public:
		std::string	stringify() const;
	};

	static std::string	stringify(const bool &boolean);
	static std::string	stringify(const char *str);
	static std::string	stringify(const std::string &str);
	template <typename T>
	static std::string	stringify(const std::vector<T> &vec);
	template <typename T>
	static std::string	stringify(const T &value);
};

std::ostream	&operator<<(std::ostream &os, const JSON::Value &value);
std::ostream	&operator<<(std::ostream &os, const JSON::Object &obj);

#include "class/JSON.tpp"

#endif
