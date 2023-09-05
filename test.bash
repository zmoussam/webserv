#!/bin/bash

HOST="localhost"
PORT="9000"
ROOT="./www"

# Helper function to send an HTTP request and check the response
function send_request {
    request="$1"
    expected_response="$2"
    response=$(echo -e "$request" | nc "$HOST" "$PORT")
    if [[ "$response" == *"$expected_response"* ]]; then
        echo -e "\033[0;32mPASS\033[0m: $request"
    else
        echo -e "\033[0;31mFAIL\033[0m: $request"
        echo -e "Expected: $expected_response"
        echo -e "Received: $response"
    fi
}

# Test cases
# Test root path
send_request "GET / HTTP/1.1\r\n\r\n" "HTTP/1.1 200 OK"

# Test a non-existent file
send_request "GET /nonexistent.html HTTP/1.1\r\n\r\n" "HTTP/1.1 404 Not Found"

# Test /static path
send_request "GET /static/ HTTP/1.1\r\n\r\n" "HTTP/1.1 301 Moved Permanently"

# Test /static/index.html
send_request "GET /static/index.html HTTP/1.1\r\n\r\n" "HTTP/1.1 200 OK"

# Test POST request to /static
send_request "POST /static/ HTTP/1.1\r\n\r\n" "HTTP/1.1 200 OK"


# Test a request to a non-existent location
send_request "GET /nonexistent-location/ HTTP/1.1\r\n\r\n" "HTTP/1.1 404 Not Found"

# Test an unsupported HTTP method
send_request "PUT / HTTP/1.1\r\n\r\n" "HTTP/1.1 405 Method Not Allowed"

# Test a request with a malformed header
send_request "GET / HTTP/1.1\r\nHost $HOST\r\n\r\n" "HTTP/1.1 400 Bad Request"

# Test a request with an invalid HTTP version
send_request "GET / HTTP/1.0\r\n\r\n" "HTTP/1.1 505 HTTP Version Not Supported"

# Test DELETE request
send_request "DELETE / HTTP/1.1\r\n\r\n" "HTTP/1.1 200 OK"

# Test UNKNOWN request
send_request "UNKNOWN / HTTP/1.1\r\n\r\n" "HTTP/1.1 400 Bad Request"

# Upload a file to the server and get it back
echo -e "This is a test file." > test.txt
send_request "POST / HTTP/1.1\r\nContent-Length: 20\r\n\r\nThis is a test file." "HTTP/1.1 200 OK"
send_request "GET /test.txt HTTP/1.1\r\n\r\n" "HTTP/1.1 200 OK"
rm -f test.txt

# Test CGI (assuming /cgi-bin/test.cgi is a CGI script)
send_request "GET /cgi-bin/test.cgi HTTP/1.1\r\n\r\n" "HTTP/1.1 200 OK"

# Test CGI with POST method
send_request "POST /cgi-bin/test.cgi HTTP/1.1\r\n\r\n" "HTTP/1.1 200 OK"

# Test CGI with error handling (CGI script with errors)
send_request "GET /cgi-bin/error.cgi HTTP/1.1\r\n\r\n" "HTTP/1.1 500 Internal Server Error"


echo "Testing complete."
