# include <string>
# include "Macros.hpp"

std::string constructFilePath(const std::string& requestPath) {
    std::string pathWithoutQuery = requestPath;
    if (pathWithoutQuery.empty() || pathWithoutQuery[0] != '/') {
        throw std::invalid_argument("Invalid request path");
    }


    std::size_t queryPos = pathWithoutQuery.find('?');
    pathWithoutQuery = (queryPos != std::string::npos) ? pathWithoutQuery.substr(0, queryPos) : pathWithoutQuery;

    if (pathWithoutQuery.back() == '/') {
        pathWithoutQuery += "index.html";
    }
    // if at the end of the path there is no slash and there is no extension then add /index.html
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