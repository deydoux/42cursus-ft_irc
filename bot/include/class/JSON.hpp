#ifndef JSON_HPP
#define JSON_HPP

#include <map>
#include <string>

class JSON
{
public:
	class SimpleObject
	{
	public:
		std::string &operator[](const std::string &key);

		std::string stringify() const;

	private:
		typedef std::map<std::string, std::string> _data_t;

		_data_t _data;
	};

	static std::string	stringify(const std::string &str);
	static std::string	stringify(const size_t &num);
};

#endif
