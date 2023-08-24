# include <string>
# include "Macros.hpp"
# include <stdexcept>

std::string constructFilePath(const std::string& requestPath) {
    std::string pathWithoutQuery = requestPath;
    if (pathWithoutQuery.empty() || pathWithoutQuery[0] != '/') {
        throw std::invalid_argument("Invalid request path");
    }
    if (!pathWithoutQuery.empty() && pathWithoutQuery[pathWithoutQuery.length() - 1] == '/') {
        pathWithoutQuery += "index.html";
    }
    // Check if there is no extension by finding the last dot in the string
    else if (pathWithoutQuery.find_last_of('.') == std::string::npos) {
        pathWithoutQuery += "/index.html";
    }

    return ROOT_PATH + pathWithoutQuery;
}


std::string getContentType(std::string filename) {
	std::string extension = filename.substr(filename.find_last_of('.'));
	if (mimeTypes.find(extension) != mimeTypes.end()) {
		return mimeTypes[extension];
	}
	return "text/html";
}