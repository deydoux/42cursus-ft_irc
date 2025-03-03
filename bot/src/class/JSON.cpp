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

void JSON::_skip_spaces(const std::string &str, size_t &pos)
{
	while (std::isspace(str[pos]))
		pos++;
}

JSON::Value JSON::_parse_value(const std::string &str, size_t &pos, bool next)
{
	_skip_spaces(str, pos);

	if (str[pos] == '"')
		return Value(_parse_string(str, pos, next));
	else if (str[pos] == '[')
		return Value(_parse_array(str, pos, next));
	else if (str[pos] == '{')
		return Value(_parse_object(str, pos, next));
	else if (str.substr(pos, 4) == "true") {
		pos += 4;
		return Value(true);
	}
	else if (str.substr(pos, 5) == "false") {
		pos += 5;
		return Value(false);
	}
	else if (str.substr(pos, 4) == "null") {
		pos += 4;
		return Value();
	}
	else
		return Value(_parse<double>(str, pos, next));

	throw Exception("Invalid JSON value");
}

std::string JSON::_parse_string(const std::string &str)
{
	static const std::string exception_message = "Invalid JSON string";

	if (str.size() < 2 || str[0] != '"' || str[str.size() - 1] != '"')
		throw Exception(exception_message);

	std::string value = str.substr(1, str.size() - 2);

	for (size_t pos = 0; pos < value.size(); pos++) {
		if (value[pos] == '\\') {
			if (pos + 1 >= value.size())
				throw Exception(exception_message);

			char next = value[pos + 1];

			if (next == '\\')
				value.replace(pos, 2, "\\");
			else if (next == '"')
				value.replace(pos, 2, "\"");
			else if (next == 'n')
				value.replace(pos, 2, "\n");
			else
				pos++;
		} else if (value[pos] == '"')
			throw Exception(exception_message);
	}

	return value;
}

std::string JSON::_parse_string(const std::string &str, size_t &pos, bool next)
{
	_skip_spaces(str, pos);
	size_t start = pos++;

	while (pos < str.size() && (str[pos] != '"' || str[pos - 1] == '\\'))
		pos++;

	if (!next) {
		size_t end = pos;
		_skip_spaces(str, end);

		if (end + 1 < str.size())
			throw Exception("Invalid JSON string");
	}

	return _parse_string(str.substr(start, ++pos - start));
}

JSON::Array JSON::_parse_array(const std::string &str)
{
	static const std::string exception_message = "Invalid JSON array";

	if (str.size() < 2 || str[0] != '[' || str[str.size() - 1] != ']')
		throw Exception(exception_message);

	Array arr;
	size_t pos = 1;

	while (pos < str.size()) {
		_skip_spaces(str, pos);
		if (pos + 1 == str.size())
			break;

		if (!arr.empty())
			if (str[pos++] != ',')
				throw Exception(exception_message);

		arr.push_back(_parse_value(str, pos, true));
	}

	if (pos + 1 < str.size())
		throw Exception(exception_message);

	return arr;
}

JSON::Array JSON::_parse_array(const std::string &str, size_t &pos, bool next)
{
	_skip_spaces(str, pos);
	size_t start = pos++;

	size_t depth = 1;
	while (pos < str.size() && depth > 0) {
		if (str[pos] == '[')
			depth++;
		else if (str[pos] == ']')
			depth--;

		pos++;
	}

	if (!next) {
		size_t end = pos;
		_skip_spaces(str, end);

		if (end + 1 < str.size())
			throw Exception("Invalid JSON array");
	}

	return _parse_array(str.substr(start, pos - start));
}

JSON::Object JSON::_parse_object(const std::string &str)
{
	static const std::string exception_message = "Invalid JSON object";

	if (str.size() < 2 || str[0] != '{' || str[str.size() - 1] != '}')
		throw Exception(exception_message);

	Object obj;
	size_t pos = 1;

	while (pos < str.size()) {
		_skip_spaces(str, pos);
		if (pos + 1 == str.size())
			break;

		if (!obj.empty())
			if (str[pos++] != ',')
				throw Exception(exception_message);

		std::string key = _parse_string(str, pos, true);

		_skip_spaces(str, pos);
		if (str[pos++] != ':')
		throw Exception(exception_message);

		obj[key] = _parse_value(str, pos, true);
	}

	if (pos + 1 < str.size())
		throw Exception(exception_message);

	return obj;
}

JSON::Object JSON::_parse_object(const std::string &str, size_t &pos, bool next)
{
	_skip_spaces(str, pos);
	size_t start = pos++;

	size_t depth = 1;
	while (pos < str.size() && depth > 0) {
		if (str[pos] == '{')
			depth++;
		else if (str[pos] == '}')
			depth--;

		pos++;
	}

	if (!next) {
		size_t end = pos;
		_skip_spaces(str, end);

		if (end + 1 < str.size())
			throw Exception("Invalid JSON object");
	}

	return _parse_object(str.substr(start, pos - start));
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
