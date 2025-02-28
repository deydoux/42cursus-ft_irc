#include "class/JSON.hpp"

#include <sstream>

std::string &JSON::SimpleObject::operator[](const std::string &key)
{
	return _data[key];
}

std::string JSON::SimpleObject::stringify() const
{
	std::ostringstream oss;
	oss << "{";

	for (_data_t::const_iterator it = _data.begin(); it != _data.end(); ++it) {
		if (it != _data.begin())
			oss << ",";

		oss << "\"" << it->first << "\":" << it->second << "";
	}

	oss << "}";
	return oss.str();
}

std::string JSON::stringify(const std::string &str)
{
	std::string result = str;

	return "\"" + result + "\"";
}
