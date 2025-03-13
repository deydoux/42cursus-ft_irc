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

		bool	operator==(const Value &rhs) const;
		template <typename T>
		bool	operator==(const T &rhs) const;
		template <typename T>
		bool	operator!=(const T &rhs) const;

		std::string	stringify() const;

		template <typename T>
		T	parse() const;

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

	class Exception : public std::invalid_argument
	{
	public:
		Exception(const std::string &message);
	};

	static std::string	stringify(const bool &boolean);
	static std::string	stringify(const char *str);
	static std::string	stringify(const std::string &str);
	template <typename T>
	static std::string	stringify(const std::vector<T> &vec);
	template <typename T>
	static std::string	stringify(const T &value);

	template <typename T>
	static T	parse(const std::string &str);

private:
	static void	_skip_spaces(const std::string &str, size_t &pos);

	static Value		_parse_value(const std::string &str, size_t &pos, bool next = false);
	static std::string	_parse_string(const std::string &str);
	static std::string	_parse_string(const std::string &str, size_t &pos, bool next = false);
	static Array		_parse_array(const std::string &str);
	static Array		_parse_array(const std::string &str, size_t &pos, bool next = false);
	static Object		_parse_object(const std::string &str);
	static Object		_parse_object(const std::string &str, size_t &pos, bool next = false);

	template <typename T>
	static T	_parse(const std::string &str, size_t &pos, bool next = false);
};

std::ostream	&operator<<(std::ostream &os, const JSON::Value &value);
std::ostream	&operator<<(std::ostream &os, const JSON::Array &array);
std::ostream	&operator<<(std::ostream &os, const JSON::Object &obj);

#include "class/JSON.tpp"

#endif
