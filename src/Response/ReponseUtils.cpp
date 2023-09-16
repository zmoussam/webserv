# include <string>
# include "Macros.hpp"
# include <stdexcept>
# include "Config.hpp"
# include <sys/stat.h>
std::string constructFilePath(const std::string& requestPath, const std::string &root, const std::string &index) {
    std::string pathWithoutQuery = requestPath;
    if (pathWithoutQuery.empty() || pathWithoutQuery[0] != '/') {
        pathWithoutQuery = "/" + pathWithoutQuery;
    }
    if (!pathWithoutQuery.empty() && pathWithoutQuery[pathWithoutQuery.length() - 1] == '/') {
        pathWithoutQuery += index;
    }
    // Check if there is no extension by finding the last dot in the string
    else if (pathWithoutQuery.find_last_of('.') == std::string::npos) {
        pathWithoutQuery += "/" + index;
    }

    std::string filePath = root + pathWithoutQuery;
    return filePath;
}


std::string getContentType(std::string filename) {
    std::string extension;
    if (filename.empty()) {
        return "text/html";
    }
    try {
	    extension = filename.substr(filename.find_last_of('.'));
    }
    catch (std::exception &e) {
        return "text/html";
    }
	if (mimeTypes.find(extension) != mimeTypes.end()) {
		return mimeTypes[extension];
	}
	return "text/html";
}

bool isDirectory(const char* path) {
    struct stat fileInfo;
    
    if (stat(path, &fileInfo) != 0) {
        return false;
    }
    
    return S_ISDIR(fileInfo.st_mode);
}