#!/usr/bin/python3

# Import modules for CGI handling 
import cgi, cgitb , html , os
# cgitb.enable()
# parse the HTTP cookies sent by the client
cookie_string = os.environ.get('HTTP_COOKIE')

if cookie_string:
    index = cookie_string.find("=")
    color = cookie_string[index+1:]
else:
    color = "white"
# access the value of a cookie named "my_cookie"
# Create instance of FieldStorage 
form = cgi.FieldStorage()

# Get data from fields
# Get data from fields
# if form.getvalue('HTTP_COOKIE'):
#     color = cookie_string
if form.getvalue('color') and form.getvalue('color') != color:
    color = form.getvalue('color')


print("Content-Type:text/html\r\n")
print("Server:Python HTTP Server\r\n")
print("Set-Cookie:color=%s\r\n" % (color))
print("\r\n\r\n")
bodystring = "<html><head><body style=\"background-color: %s;\"></head><body>" % (color)
bodystring += "<h1>Welcome to my page</h1>"
bodystring += "<form method=\"GET\" action=\"cookie.py\">"
bodystring += "<label for=\"color\">Background Color:</label>"
bodystring += "<input type=\"text\" name=\"color\" id=\"color\">"
bodystring += "<input type=\"submit\" value=\"Set Color\">"
bodystring += "</form>"
bodystring += "</body>"
bodystring += "</html>"
print (bodystring)