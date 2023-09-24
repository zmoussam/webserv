#!/usr/bin/python3
import cgi
# while True:
#    continue
form = cgi.FieldStorage()
if form.getvalue('textcontent'):
   text_content = form.getvalue('textcontent')
else:
   text_content = "Not entered"
body_string = """<html><head><title>Hello - Second CGI Program</title></head><body><h2> %s </h2></body></html>""" % (text_content)

print (body_string)