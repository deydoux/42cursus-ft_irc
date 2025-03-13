#ifndef JSON_TPP
#define JSON_TPP

#include "class/JSON.hpp"

#include <sstream>
#include <typeinfo>

template <typename T>
JSON::Value::Value(const T &value) : _str(JSON::stringify(value)) {}

template <typename T>
JSON::Value &JSON::Value::operator=(const T &rhs)
{
	_str = JSON::stringify(rhs);
	return *this;
}

template <typename T>
bool JSON::Value::operator==(const T &rhs) const
{
	return *this == Value(rhs);
}

template <typename T>
bool JSON::Value::operator!=(const T &rhs) const
{
	return !(*this == Value(rhs));
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

template <>
inline bool JSON::parse<bool>(const std::string &str)
{
	if (str.substr(0, 4) == "true")
		return true;
	else if (str.substr(0, 5) == "false")
		return false;
	else
		throw Exception("Invalid JSON boolean");
}

template <>
inline std::string JSON::parse<std::string>(const std::string &str)
{
	size_t pos = 0;
	return _parse_string(str, pos);
}

template <>
inline JSON::Array JSON::parse<JSON::Array>(const std::string &str)
{
	size_t pos = 0;
	return _parse_array(str, pos);
}

template <>
inline JSON::Object JSON::parse<JSON::Object>(const std::string &str)
{
	size_t pos = 0;
	return _parse_object(str, pos);
}

template <typename T>
T JSON::parse(const std::string &str)
{
	size_t pos = 0;
	return _parse<T>(str, pos);
}

template <typename T>
T JSON::_parse(const std::string &str, size_t &pos, bool next)
{
	std::istringstream iss(str.substr(pos));

	T value;
	iss >> value;

	if ((!next && !iss.eof()) || iss.fail())
		throw Exception("Invalid JSON value");

	pos += iss.tellg();
	return value;
}

#endif
