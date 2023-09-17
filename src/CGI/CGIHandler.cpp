# include "../../inc/CGIHandler.hpp"
# include <fcntl.h>
CGI::CGI()
{
  _root = "www/";
  _autoindex = false;
  _redirect = "";
  _cgi_path = "";
  _compiler = "/usr/bin/python3 ";
  _headersSent = false;
  _error_code = 0;
  _status_code = "";
  _isCgiDone = false;
  _cgiRan = false;
  _fd = 0;
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
    {
      _error_code = 404;
      std::cout << "Error: extension empty" << std::endl;
    }
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
    {

      _error_code = 404;
      std::cout << "Error: compiler empty" << std::endl;
    }
  initHeaders();
}

void CGI::handlePostMethod(Request &req, std::string &tmpfile) {
  if (req.getMethod() == "POST")
  {
    std::string body = req.getBody();
    std::ofstream ofs(tmpfile);
		if (!ofs.is_open())
			{
        _error_code = 503;
        std::cout << "Error:wik"<< std::endl;
        }
		ofs << body;
		ofs.close();
		int fdf = open(tmpfile.c_str(), O_RDWR);
		if (fdf == -1)
			{
        _error_code = 503;
        std::cout << "Error:lk"<< std::endl;
        }
		if (dup2(fdf, STDIN_FILENO) == -1)
			{
        _error_code = 503;
        std::cout << "Error:fik"<< std::endl;
        }
		close(fdf);
    if (req.getHeaders().find("Content-Length") != req.getHeaders().end())
      setenv("CONTENT_LENGTH", req.getHeaders()["Content-Length"].c_str(), 1);
  }
}

void CGI::executeCGIScript(int clientSocket) {
  unused(clientSocket);
  std::string command = _compiler + _root + _filename;
  FILE* pipe = popen(command.c_str(), "r");
  if (!pipe)
    _error_code = 503;
  char buffer[128];
    std::string body;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        body += buffer;
    pclose(pipe);
    
    std::ofstream file("Lina.txt");
    if (file.is_open()) {
      file << body;
      file.close();
    } else {
      _error_code = 503;
    }
    // file.close();
}

int CGI::CGIHandler(Request &req, Response &resp, int clientSocket)
{
  std::cout << "fdlwl: " << _fd << std::endl;
  initializeCGIParameters(req, resp);
  int status = 0;
  int pid = 0;
  if (_cgiRan == false) {
    pid = fork();
    if (pid == 0) {
    std::string tmpfile = std::to_string(getpid()) + ".txt";
    std::cout << "dkhlt: "<< std::endl;
      handlePostMethod(req, tmpfile);
      unlink(tmpfile.c_str());
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
      exit(DONE);
    }
  }
  if (waitpid(pid, &status, 0) == -1)
  {
    _error_code = 503;
    std::cout << "Error: waitpid" << std::endl;
  }
  _cgiRan = true;
  if (WIFEXITED(status)) {
    status = WEXITSTATUS(status);
    _isCgiDone = true;
    if (_fd == 0)
      _fd = open("Lina.txt", O_RDONLY);
    std::cout << "fd: " << _fd << std::endl;
    if (_fd == -1) {
      _error_code = 503;
    }
  }
  else {
    status = CONTINUE;
  }
  return (status);
}