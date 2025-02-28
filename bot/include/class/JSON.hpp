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
		Value(const T &other);

		Value	&operator=(const Value &rhs);
		template <typename T>
		Value	&operator=(const T &rhs);

		std::string	stringify() const;

	private:
		std::string _str;
	};

	class Object
	{
	public:
		Value	&operator[](const std::string &key);

		std::string	stringify() const;

	private:
		typedef std::string				_key_t;
		typedef std::map<_key_t, Value>	_data_t;

		_data_t _data;
	};

	static std::string	stringify(const bool &boolean);
	static std::string	stringify(const char *str);
	static std::string	stringify(const std::string &str);
	template <typename T>
	static std::string	stringify(const std::vector<T> &vec);
	template <typename T>
	static std::string	stringify(const T &other);
};

std::ostream	&operator<<(std::ostream &os, const JSON::Value &value);
std::ostream	&operator<<(std::ostream &os, const JSON::Object &obj);

#include "class/JSON.tpp"

#endif
