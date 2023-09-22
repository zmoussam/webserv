#!/usr/bin/env python

import cgi
import os

# Function to clear the session cookie
def clear_session_cookie():
    print("Set-Cookie: session_id=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT")

# Main CGI script
def main():
    # Clear the session cookie to log the user out
    clear_session_cookie()

    # HTML content
    print("Content-Type: text/html\r\n")
    print("\r\n\r\n")
    print("<html><head><title>Logout</title></head><body>")
    print("<h1>Logout</h1>")
    print("<p>You have been logged out.</p>")
    print("<a href='login.py'>Return to Login Page</a>")
    print("</body></html>")

if __name__ == "__main__":
    main()
