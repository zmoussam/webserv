#!/usr/bin/env python

import cgi
import cgitb
import os
import base64
import hashlib
import sys

# Enable error traceback for debugging (comment this out in production)
cgitb.enable()

# Define a dictionary to store user credentials (replace with a database in production)
user_credentials = {
    'admin': 'admin',
    'user2': 'password2',
}

# Function to hash a string using MD5
def md5_hash(text):
    return hashlib.md5(text.encode()).hexdigest()

# Function to encode a session ID as Base64
def encode_session_id(username, password):
    session_id = "{}:{}".format(username, password)
    return base64.b64encode(session_id.encode()).decode()

# Function to set a session cookie with the Base64-encoded session ID
def set_session_cookie(session_id):
    print("Set-Cookie: session_id={}".format(session_id))

# Function to get the session ID from cookies
def get_session_id():
    cookies = os.environ.get("HTTP_COOKIE")
    if cookies:
        cookies = cookies.split("; ")
        for cookie in cookies:
            # split the cookie and ignore the = in the value of the cookie
            name, value = cookie.split("=", 1)
            if name == "session_id":
                return value
    return None

# Function to decode a session ID from Base64
def decode_session_id(session_id):
    decoded_session_id = base64.b64decode(session_id).decode()
    return decoded_session_id.split(":")

# Function to check if a user is logged in
def is_user_logged_in(session_id):
    return session_id is not None

# Main CGI script
def main():
    # Get the session ID from cookies or generate a new one
    session_id = get_session_id()
    if not session_id:
        session_id = None
    username = None
    password = None

    if session_id:
        # Decode session ID
        decoded_session_id = decode_session_id(session_id)
        if len(decoded_session_id) == 2:
            username, password = decoded_session_id
            if username not in user_credentials or user_credentials[username] != password:
                username = None

    # HTML content
    print("Content-Type: text/html\r\n")
    form = cgi.FieldStorage()

    if "username" in form and "password" in form:
        username = form.getvalue("username")
        password = form.getvalue("password")
        if username in user_credentials and user_credentials[username] == password:
            # Encode username and password as the session ID
            session_id = encode_session_id(username, password)
            set_session_cookie(session_id)
    print("\r\n\r\n")
    print("<html><head><title>Login Page with CGI and Sessions</title></head><body>")
    print("<h1>Login Page with CGI and Sessions</h1>")


    if is_user_logged_in(session_id):
        print("<p>You are logged in as {}.</p>".format(username))
        print("<form method='post' action='logout.py'>")
        print("<input type='submit' value='Logout'>")
        print("</form>")
        print("""<iframe width="560" height="315" src="https://www.youtube.com/embed/dQw4w9WgXcQ?si=MfJvn7xuVwIWHr_M" title="YouTube video player" frameborder="1" allow="accelerometer; autoplay="1"; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>""")
    else:
        print("<form method='post'>")
        print("<label for='username'>Username:</label>")
        print("<input type='text' id='username' name='username'><br>")
        print("<label for='password'>Password:</label>")
        print("<input type='password' id='password' name='password'><br>")
        print("<input type='submit' value='Login'>")
        print("</form>")

    print("</body></html>")

if __name__ == "__main__":
    main()
