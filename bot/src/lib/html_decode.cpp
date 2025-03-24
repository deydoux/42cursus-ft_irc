#include <map>
#include <string>

std::string html_decode(const std::string &str)
{
	static std::map<std::string, std::string> entities;

	if (entities.empty()) {
		entities["&#039;"] = "'";
		entities["&acirc;"] = "â";
		entities["&agrave;"] = "à";
		entities["&amp;"] = "&";
		entities["&ccedil;"] = "ç";
		entities["&eacute;"] = "é";
		entities["&ecirc;"] = "ê";
		entities["&egrave;"] = "è";
		entities["&euml;"] = "ë";
		entities["&gt;"] = ">";
		entities["&icirc;"] = "î";
		entities["&iuml;"] = "ï";
		entities["&ldquo;"] = "“";
		entities["&lt;"] = "<";
		entities["&ocirc;"] = "ô";
		entities["&oelig;"] = "œ";
		entities["&ouml;"] = "ö";
		entities["&quot;"] = "\"";
		entities["&rdquo;"] = "”";
		entities["&rsquo;"] = "’";
		entities["&ucirc;"] = "û";
		entities["&ugrave;"] = "ù";
		entities["&uuml;"] = "ü";
	}

	std::string result = str;

	for (std::map<std::string, std::string>::const_iterator it = entities.begin(); it != entities.end(); ++it) {
		size_t pos = 0;
		while ((pos = result.find(it->first, pos)) != std::string::npos) {
			result.replace(pos, it->first.size(), it->second);
			pos += it->second.size();
		}
	}

	return result;
}
