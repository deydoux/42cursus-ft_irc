#ifndef CURL_HPP
#define CURL_HPP

#include <stdexcept>
#include <string>

#include <curl/curl.h>

class Curl
{
public:
	Curl();
	~Curl();

	std::string get(const std::string &url);
	std::string post(const std::string &url, const std::string &data);

	class Exception : public std::runtime_error
	{
	public:
		Exception(const std::string &message);
	};

private:
	CURL		*_handle;
	std::string	_buffer;

	void		_reset();
	CURLcode	_setopt(CURLoption option, const void *data);
	CURLcode	_setopt_writefunction();

	void		_init(const std::string &url);
	std::string	_perform();

	static size_t _write_callback(char *ptr, size_t size, size_t nmemb, std::string *userdata);
};

#endif
