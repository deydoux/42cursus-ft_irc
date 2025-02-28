#ifndef JSON_TPP
#define JSON_TPP

#include "class/JSON.hpp"

#include <sstream>
#include "JSON.hpp"

template <typename T>
JSON::Value::Value(const T &value) : _str(JSON::stringify(value)) {}

template <typename T>
JSON::Value &JSON::Value::operator=(const T &rhs)
{
	_str = JSON::stringify(rhs);
	return *this;
}

template <typename T>
T JSON::Value::parse() const
{
	return JSON::parse<T>(_str);
}

template <typename T>
std::string JSON::stringify(const std::vector<T> &vec)
{
	std::string result = "[";

	for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
		if (it != vec.begin())
			result += ",";
		result += JSON::stringify(*it);
	}

	result += "]";
	return result;
}

template <typename T>
std::string JSON::stringify(const T &value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

template <typename T>
T JSON::parse(const std::string &str)
{
	std::istringstream iss(str);

	T value;
	iss >> value;

	if (!iss.eof() || iss.fail())
		throw Exception("Invalid JSON value");

	return value;
}

#endif
