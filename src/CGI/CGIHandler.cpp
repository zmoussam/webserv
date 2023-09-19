# include "../../inc/CGIHandler.hpp"
CGI::CGI()
{
  _root = "www/";
  _redirect = "";
  _cgi_path = "";
  _compiler = "/usr/bin/python3 ";
  _headersSent = false;
  _error_code = 0;
  _status_code = "";
  _isCgiDone = false;
  _cgiRan = false;
  _fd = 0;
  _pid = 0;
  _status = 0;
}

CGI::CGI(int clientSocket, std::vector<ServerConf> &servers) : _servers(servers)
{
  _root = "www/";
  _redirect = "";
  _cgi_path = "";
  _compiler = "";
  _clientSocket = clientSocket;
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
  _headers["Server"] = "luna/1.0";
  _headers["Content-Type"] = "text/html";
  if (!(_redirect.empty())) {
    _headers["Location"] = _redirect;
    _error_code = 301;
  }
  _headers["Connection"] = "keep-alive";
}

void    CGI::findConfig(Request &req)
{
    const std::map<std::string, std::string> &headers = req.getHeaders();
    if (headers.find("Host") != headers.end())
    {
        std::string host = headers.find("Host")->second;
        for (std::vector<ServerConf>::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            if (it->getString(SERVER_NAME) == host)
            {
                _config = *it;
                return;
            }
        }
    }
    _config = _servers[0];
}

int CGI::initializeCGIParameters(Request &req, Response &resp) {
  findConfig(req);
  _cgi_path = req.getPath();
  _filename = _cgi_path.substr(_cgi_path.find_last_of('/') + 1);
  std::string fileExtension = "";
  std::vector<Location>::iterator it = _config.location.begin();
  std::string extension = getFileExtension(_filename);
  if (extension.empty())
  {
    _error_code = 500;
    return 500;
  }
  while (it != resp._config.location.end())
  {
    std::string cgiLocation = it->getLocationName().erase(0, 1);
    if (cgiLocation == extension)
    {
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
  {
    _error_code = 405;
    std::cout << "error code: " << _error_code << std::endl;
    return 405;
  }
  if (_compiler.empty())
  {
    _error_code = 500;
    return 500;
  }
  initHeaders();
  return 0;
}

int CGI::handlePostMethod(Request &req) {
  if (req.getMethod() == "POST")
  {
    std::string tmpfile = std::to_string(getpid()) + ".txt";
    std::string body = req.getBody();
    std::ofstream ofs(tmpfile);
		if (!ofs.is_open()) {
      _error_code = 500;
      return 500;
    }
		ofs << body;
		ofs.close();
		int fdf = open(tmpfile.c_str(), O_RDWR);
		if (fdf == -1) {
      _error_code = 500;
      return 500;
    }
		if (dup2(fdf, STDIN_FILENO) == -1) {
      _error_code = 500;
      return 500;
    }
		close(fdf);
    if (req.getHeaders().find("Content-Length") != req.getHeaders().end())
      setenv("CONTENT_LENGTH", req.getHeaders()["Content-Length"].c_str(), 1);
    unlink(tmpfile.c_str());
  }
  return 0;
}

int CGI::executeCGIScript(int clientSocket) {
    unused(clientSocket);
    std::string path = _root + _filename;
    if (access(path.c_str(), F_OK) == -1) {
        _error_code = 404;
        return 404;
    }
    std::string command = _compiler + _root + _filename;
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        _error_code = 500;
        return 500;
    }
    
    char buffer[128];
    std::string body;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL)
        body += buffer;
    pclose(pipe);
    
    if (_fd != -1) {
        write(_fd, body.c_str(), body.size());
    } else {
        _error_code = 500;
        return 500;
    }
    return 0;
}

int CGI::CGIHandler(Request &req, Response &resp, int clientSocket)
{

  if (_cgiRan == false) {
    int random = open("/dev/random", O_RDONLY);
    if (random == -1) {
      _error_code = 500;
      checkStatusCode();
      return (CONTINUE);
    }
    read(random, &random, sizeof(random));
    close(random);
    _cgifd = "/tmp/" + std::to_string(random) + ".cgi";
    _pid = fork();
    _fd = open(_cgifd.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (_pid == 0) {
      if (initializeCGIParameters(req, resp) != 0)
        exit (_error_code);
      if (handlePostMethod(req) != 0)
        exit (_error_code);
      setenv("HTTP_COOKIE", req.getHeaders()["Cookie"].c_str(), 1);
      setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
      setenv("REQUEST_URI", req.getPath().c_str(), 1);
      setenv("QUERY_STRING", req.getQueries().c_str(), 1);
      setenv("CONTENT_TYPE", req.getHeaders()["Content-Type"].c_str(), 1);
      setenv("SCRIPT_NAME", req.getPath().c_str(), 1);
      setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
      setenv("PATH_INFO", req.getPath().c_str(), 1);
      setenv("SCRIPT_NAME", _cgi_path.c_str(), 1);
      setenv("REDIRECT_STATUS", "", 1);
      if (executeCGIScript(clientSocket) != 0)
        exit (_error_code);
      std::cout << "wa exita with code: " << _error_code << std::endl;
      exit(DONE);
    }
  }
  if (waitpid(_pid, &_status, WNOHANG) == -1)
  {
      if (WIFEXITED(_status)) {
        _status = WEXITSTATUS(_status);
      _isCgiDone = true;
      checkStatusCode();
      if (_fd == -1) {
        _error_code = 500;
        checkStatusCode();
      }
    }
    else
      return (CONTINUE);
  }
  _cgiRan = true;
  return (CONTINUE);
}