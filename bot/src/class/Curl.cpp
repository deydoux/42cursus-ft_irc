#include "class/Curl.hpp"

Curl::Curl() : _handle(curl_easy_init())
{
	if (!_handle) throw std::runtime_error("Failed to initialize curl");
}

Curl::~Curl()
{
	if (_handle) curl_easy_cleanup(_handle);
}

Curl::Exception::Exception(const std::string &message) : std::runtime_error(message) {}

std::string Curl::get(const std::string &url)
{
	_init(url);

	return _perform();
}

std::string Curl::post(const std::string &url, const std::string &data)
{
	_init(url);
	_setopt(CURLOPT_POSTFIELDS, data.c_str());

	return _perform();
}

void Curl::_reset()
{
	_buffer.clear();
	curl_easy_reset(_handle);
}

CURLcode Curl::_setopt(CURLoption option, const void *data)
{
	return curl_easy_setopt(_handle, option, data);
}

CURLcode Curl::_setopt_writefunction()
{
	return curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, &_write_callback);
}

void Curl::_init(const std::string &url)
{
	_reset();

	_setopt(CURLOPT_URL, url.c_str());
	_setopt_writefunction();
	_setopt(CURLOPT_WRITEDATA, &_buffer);
}

std::string Curl::_perform()
{
	CURLcode res = curl_easy_perform(_handle);

	if (res != CURLE_OK) throw Curl::Exception(curl_easy_strerror(res));

	return _buffer;
}

size_t Curl::_write_callback(char *ptr, size_t size, size_t nmemb, std::string *userdata)
{
	userdata->append(ptr, size * nmemb);
	return size * nmemb;
}

