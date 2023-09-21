import cgi

form = cgi.FieldStorage()


try:
	name = form['name'].value
	email = form['email'].value
	subject = form['subject'].value
	message = form['message'].value
	print("<html>")
	print("<head>")
	print("</head>")
	print("<body>")
	print("<header>")
	print("</header>")
	print("<div class='container'>")
	print("<div class='result'>")
	print("<h2>Form Result</h2>")
	# print("<p>Name: {name}</p>")
	print("<p>Name:")
	print(name)
	print("</p>")
	print("<p>Email:")
	print(email)
	print("</p>")
	print("<p>Subject:")
	print(subject)
	print("</p>")
	print("<p>Message:")
	print(message)
	print("</p>")
	# print("<p>Email: {email}</p>")
	# print("<p>Subject: {subject}</p>")
	# print("<p>Message: {message}</p>")
	print("</div>")
	print("</div>")
	print("</body>")
	print("</html>")
except ValueError:
	print("<p>Error: Please enter a valid number</p>")

    