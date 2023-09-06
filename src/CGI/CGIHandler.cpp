# include "CGIHandler.hpp"
# include <iostream>
# include <fcntl.h>
CGI::CGI()
{
  _root = "www/";
  _autoindex = false;
  _redirect = "";
  _cgi_path = "";
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

int CGI::CGIHandler(Request &req, Response &resp, int clientSocket)
{
  int pid;
  _cgi_path = req.getPath();
  std::string fileExtension = "";
  std::string file = _cgi_path.substr(_cgi_path.find_last_of('/') + 1);
  std::vector<Location>::iterator it = resp._config.location.begin();
  std::string extension = getFileExtension(file);
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
      _root = it->getString(ROOT);
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
    throw 405;
  std::string command = "/usr/bin/python3 " + _root + file;
  std::string filexec = _root + _cgi_path.substr(_cgi_path.find_last_of('/') + 1);
  std::string tmpfile = std::to_string(getpid()) + ".txt";
  pid = fork();
  if (pid == 0) {
    if (req.getMethod() == "POST")
    {
        std::string body = req.getBody().substr(req.getBody().find_last_of("=") + 1);
        int fd = ::open(tmpfile.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (fd != -1) {
          ssize_t bytesWritten = ::write(fd, body.c_str(), body.size());
          dup2(fd, STDIN_FILENO);
          ::close(fd);
          if (!(bytesWritten == static_cast<ssize_t>(body.size()))) {
              throw std::runtime_error("write() failed!");
          }
      }
      if (req.getHeaders().find("Content-Length") != req.getHeaders().end())
        setenv("CONTENT_LENGTH", std::to_string(body.size()).c_str(), 1);
    }
    setenv("REQUEST_METHOD", req.getMethod().c_str(), 1);
    setenv("REQUEST_URI", req.getPath().c_str(), 1);
    setenv("QUERY_STRING", req.getQueries().c_str(), 1);
    setenv("CONTENT_TYPE", req.getHeaders()["Content-Type"].c_str(), 1);
    setenv("SCRIPT_NAME", req.getPath().c_str(), 1);
    setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    setenv("PATH_INFO", req.getPath().c_str(), 1);
    setenv("SCRIPT_NAME", _cgi_path.c_str(), 1);
    setenv("REDIRECT_STATUS", "", 1);
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    char buffer[128];
    std::string bodyResp = "HTTP/1.1 200 OK\r\n"
                                "Server: luna/1.0\r\n"
                                "Content-Type: text/html\r\n\r\n";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        bodyResp += buffer;
    }
    pclose(pipe);
    ssize_t bytesSent = send(clientSocket, bodyResp.c_str(), strlen(bodyResp.c_str()), 0);
    if (bytesSent == -1)
      throw 503;
    unlink(tmpfile.c_str());
    exit(0);
  }
  else
    wait(NULL);
  unlink(tmpfile.c_str());
  return (DONE);
}
