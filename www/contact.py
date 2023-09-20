import cgi

form = cgi.FieldStorage()


try:
	name = form['name'].value
	email = form['email'].value
	subject = form['subject'].value
	message = form['message'].value
	print(f"<html>")
	print(f"<head>")
	print(f"</head>")
	print(f"<body>")
	print(f"<header>")
	print(f"</header>")
	print(f"<div class='container'>")
	print(f"<div class='result'>")
	print(f"<h2>Form Result</h2>")
	print(f"<p>Name: {name}</p>")
	print(f"<p>Email: {email}</p>")
	print(f"<p>Subject: {subject}</p>")
	print(f"<p>Message: {message}</p>")
	print(f"</div>")
	print(f"</div>")
	print(f"</body>")
	print(f"</html>")
except ValueError:
	print("<p>Error: Please enter a valid number</p>")

    