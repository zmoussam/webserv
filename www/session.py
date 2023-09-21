#!/usr/bin/env python

import cgi
import cgitb
import os
import random
import hashlib
import sys
# Enable error traceback for debugging (comment this out in production)
cgitb.enable()

# Function to generate a random session ID
def generate_session_id():
    return hashlib.sha256(os.urandom(32)).hexdigest()

# Function to set a session cookie
def set_session_cookie(session_id):
    print("Set-Cookie:session_id={}\r\n".format(session_id))

# Function to get the session ID from cookies
def get_session_id():
    cookies = os.environ.get("HTTP_COOKIE")
    if cookies:
        cookies = cookies.split("; ")
        for cookie in cookies:
            name, value = cookie.split("=")
            if name == "session_id":
                return value
    return None

# Main CGI script
def main():
    # Get the session ID from cookies or generate a new one
    print("Content-Type:text/html\r\n")
    session_id = get_session_id()
    if not session_id:
        session_id = generate_session_id()
        set_session_cookie(session_id)
    print("\r\n\r\n")
    # HTML content
    print("<html><head><title>Session Handling with Cookies</title></head><body>")
    print("<h1>Session Handling with Cookies</h1>")
    print("<p>Session ID: {}</p>".format(session_id))
    print("<p>This is a simple example of session handling using cookies.</p>")
    print("<p>Refresh the page to see the session ID persist.</p>")
    print("</body></html>")

if __name__ == "__main__":
    main()
