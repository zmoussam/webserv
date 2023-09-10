# include "../../inc/CGIHandler.hpp"
# include <fcntl.h>
CGI::CGI()
{
  _root = "www/";
  _autoindex = false;
  _redirect = "";
  _cgi_path = "";
  _compiler = "";
  _headersSent = false;
  _error_code = 0;
  _status_code = "";
}

CGI::~CGI()
{

}
std::string getFileExtension(const std::string& filename)
{
    size_t dotPosition = filename.find_last_of(".");
    
    if (dotPosition != std::string::npos) {
        return filename.substr(dotPosition);
    }
    return "";
}

void    CGI::checkStatusCode() {
    _status_code = "200 OK";
    switch (_error_code) {
        case 301:
            _status_code = "301 Moved Permanently";
            break;
        case 404:
            _status_code = "404 Not Found";
            break;
        case 405:
            _status_code = "405 Method Not Allowed";
            break;
        case 500:
            _status_code = "500 Internal Server Error";
            break;
        case 501:
            _status_code = "501 Not Implemented";
            break;
        case 505:
            _status_code = "505 HTTP Version Not Supported";
        case 503:
            _status_code = "503 Service Unavailable";
            break;
        default:
            break;
    }
}
void CGI::initHeaders() {
  checkStatusCode();
  _headers["Server"] = "luna/1.0";
  // if (_headers.find("Content-Type") == _headers.end())
      // _headers["Content-Type"] = getContentType(_filePath);
  _headers["Content-Type"] = "text/html";
  if (!(_redirect.empty())) {
      _headers["Location"] = _redirect;
      _error_code = 301;
  }
  _headers["Connection"] = "keep-alive";
}

void CGI::initializeCGIParameters(Request &req, Response &resp) {
  _cgi_path = req.getPath();
  _filename = _cgi_path.substr(_cgi_path.find_last_of('/') + 1);
  std::string fileExtension = "";
  std::vector<Location>::iterator it = resp._config.location.begin();
  std::string extension = getFileExtension(_filename);
  if (extension.empty())
    throw 404;
  while (it != resp._config.location.end())
  {
    std::string cgiLocation = it->getLocationName().erase(0, 1);
    if (cgiLocation == extension)
    {
      _autoindex = it->getAutoindex();
      _redirect = it->getReturned();
      _methods = it->getMethods();
      if (!it->getString(ROOT).empty())
        _root = it->getString(ROOT);
      _compiler = it->getCompiler();
      _redirect = it->getReturned();
      break;
    }
    it++;
  }
  std::vector<std::string>::iterator it_meth = _methods.begin();
  while (it_meth != _methods.end())
  {
    if (*it_meth == req.getMethod())
      break;
    it_meth++;
  }
  if (it_meth == _methods.end())
    _error_code = 405;
  if (_compiler.empty())
    throw 503;
  initHeaders();
}

void CGI::handlePostMethod(Request &req, std::string &tmpfile) {
  if (req.getMethod() == "POST")
  {
    std::string body = req.getBody();
    std::ofstream ofs(tmpfile);
		if (!ofs.is_open())
			throw 503;
		ofs << body;
		ofs.close();
		int fdf = open(tmpfile.c_str(), O_RDWR);
		if (fdf == -1)
			throw 503;
		if (dup2(fdf, STDIN_FILENO) == -1)
			throw 503;
		close(fdf);
    if (req.getHeaders().find("Content-Length") != req.getHeaders().end())
      setenv("CONTENT_LENGTH", req.getHeaders()["Content-Length"].c_str(), 1);
  }
}

void CGI::executeCGIScript(int clientSocket) {
  std::string command = _compiler + _root + _filename;
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe)
    _error_code = 503;
  char buffer[128];
  std::string body;
  std::stringstream ss;
   if (_headersSent == false) {
    ss << "HTTP/1.1 " << _status_code << "\r\n";
    for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++) {
        ss << it->first << ": " << it->second << "\r\n";
    }
    ss << "\r\n";
    body = ss.str();
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        body += buffer;
    pclose(pipe);
    ssize_t bytesSent = send(clientSocket, body.c_str(), strlen(body.c_str()), 0);
    if (bytesSent == -1)
      _error_code = 503;
    }
    _headersSent = true;
  }

int CGI::CGIHandler(Request &req, Response &resp, int clientSocket)
{
  //   std::cout << req.getBody().size() << " " << atoi(req.getHeaders()["Content-Length"].c_str()) << std::endl;
  // if (req.getBody().size() <  (size_t)atoi(req.getHeaders()["Content-Length"].c_str()))
  // {
  //   return (CONTINUE);
  // }
  initializeCGIParameters(req, resp);
  std::string tmpfile = std::to_string(getpid()) + ".txt";
  int status;
  int pid = fork();
  if (pid == 0) {
    handlePostMethod(req, tmpfile);
    setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
    setenv("REQUEST_URI", req.getPath().c_str(), 1);
    setenv("QUERY_STRING", req.getQueries().c_str(), 1);
    setenv("CONTENT_TYPE", req.getHeaders()["Content-Type"].c_str(), 1);
    setenv("SCRIPT_NAME", req.getPath().c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("PATH_INFO", req.getPath().c_str(), 1);
    setenv("SCRIPT_NAME", _cgi_path.c_str(), 1);
    setenv("REDIRECT_STATUS", "", 1);
    executeCGIScript(clientSocket);
    exit(0);
  }
  else
    if (waitpid(pid, &status, WNOHANG) == -1)
      throw 503;
  unlink(tmpfile.c_str());
  return (DONE);
}