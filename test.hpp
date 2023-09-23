#include <exception>
#include <string>

class HTTPException : public std::exception {
public:
    HTTPException(const std::string& message) : message_(message) {}

    virtual const char* what() const throw() {
        return message_.c_str();
    }
    virtual ~HTTPException() throw() {}

private:
    std::string message_;
};