#include "class/JSON.hpp"

#include <sstream>

JSON::Value::Value() : _str("null") {}

JSON::Value::Value(const Value &other) : _str(other._str) {}

JSON::Value &JSON::Value::operator=(const Value &rhs)
{
	_str = rhs._str;
	return *this;
}

std::string JSON::Value::stringify() const
{
	return _str;
}

std::string JSON::Array::stringify() const
{
	std::string result = "[";

	for (const_iterator it = begin(); it != end(); ++it) {
		if (it != begin())
			result += ",";
		result += it->stringify();
	}

	result += "]";
	return result;
}

std::string JSON::Object::stringify() const
{
	std::string result = "{";

	for (const_iterator it = begin(); it != end(); ++it) {
		if (it != begin())
			result += ",";

		std::string key = it->first;
		std::string value = it->second.stringify();
		result += "\"" + key + "\":" + value;
	}

	result += "}";
	return result;
}

JSON::Exception::Exception(const std::string &message) : std::invalid_argument(message) {}

std::string JSON::stringify(const bool &boolean)
{
	return boolean ? "true" : "false";
}

std::string JSON::stringify(const char *str)
{
	if (!str)
		return "null";
	return stringify(std::string(str));
}

std::string JSON::stringify(const std::string &str)
{
	std::string result = str;

	size_t pos = 0;
	while ((pos = result.find('\\', pos)) != std::string::npos) {
		result.replace(pos, 1, "\\\\");
		pos += 2;
	}

	pos = 0;
	while ((pos = result.find('"', pos)) != std::string::npos) {
		result.replace(pos, 1, "\\\"");
		pos += 2;
	}

	result = "\"" + result + "\"";
	return result;
}

std::ostream &operator<<(std::ostream &os, const JSON::Value &value)
{
	os << value.stringify();
	return os;
}

std::ostream &operator<<(std::ostream &os, const JSON::Array &array)
{
	os << array.stringify();
	return os;
}
std::ostream &operator<<(std::ostream &os, const JSON::Object &obj)
{
	os << obj.stringify();
	return os;
}
