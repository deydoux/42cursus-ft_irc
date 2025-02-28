#ifndef JSON_TPP
#define JSON_TPP

#include "class/JSON.hpp"

#include <sstream>

template <typename T>
JSON::Value::Value(const T &other) : _str(JSON::stringify(other)) {}

template <typename T>
JSON::Value &JSON::Value::operator=(const T &rhs)
{
	_str = JSON::stringify(rhs);
	return *this;
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
std::string JSON::stringify(const T &num)
{
	std::ostringstream oss;
	oss << num;
	return oss.str();
}

#endif
