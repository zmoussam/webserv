#!/usr/bin/env python3

import cgi
form = cgi.FieldStorage()

if 'm' in form and 'n' in form:
    try:
        multiplicand1 = int(form['m'].value)
        multiplicand2 = int(form['n'].value)
        product = multiplicand1 * multiplicand2
        print("<html><body>")
        print("<h1>Multiplication Result</h1>")
        print(multiplicand1)
        print("*")
        print(multiplicand2)
        print("=")
        print(product)
        print("</body></html>")
    except ValueError:
        print("<html><body>")
        print("<h1>Error</h1>")
        print("<p>Invalid input. Please enter valid numbers.</p>")
        print("</body></html>")
else:
    print("<html><body>")
    print("<h1>Error</h1>")
    print("<p>Missing parameters. Please provide both multiplicands.</p>")
    print("</body></html>")


# #!/usr/bin/env python3

# import cgi
# form = cgi.FieldStorage()

# # Define CSS styles
# css_style = """
# <style>
#   body {
#     font-family: Arial, sans-serif;
#     background-color: #f0f0f0;
#     margin: 0;
#     padding: 0;
#   }
#   .container {
#     max-width: 800px;
#     margin: 0 auto;
#     padding: 20px;
#     background-color: #fff;
#     box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
#   }

#   p {
#     font-size: 18px;
#     line-height: 1.5;
#     margin: 10px 0; /* Add margin to the top and bottom of paragraphs */
#     color: #555; /* Change the text color of paragraphs */
#   }

#   .result {
#     background-color: #f9f9f9;
#     padding: 10px;
#     border: 1px solid #ccc;
#     margin-top: 20px;
#   }
# </style>
# """

# if 'm' in form and 'n' in form:
#     try:
#         multiplicand1 = int(form['m'].value)
#         multiplicand2 = int(form['n'].value)
#         product = multiplicand1 * multiplicand2
#         print(f"<html>")
#         print(f"<head>")
#         print(css_style)
#         print(f"</head>")
#         print(f"<body>")
#         print(f"<header>")
#         print(f"</header>")
#         print(f"<div class='container'>")
#         print(f"<div class='result'>")
#         print(f"<h2>Multiplication Result</h2>")
#         print(f"<p>{multiplicand1} * {multiplicand2} = {product}</p>")
#         print(f"</div>")
#         print(f"</div>")
#         print(f"</body>")
#         print(f"</html>")
#     except ValueError:
#         print("<html><body>")
#         print("<h1>Error</h1>")
#         print("<p>Invalid input. Please enter valid numbers.</p>")
#         print("</body></html>")
# else:
#     print("<html><body>")
#     print("<h1>Error</h1>")
#     print("<p>Missing parameters. Please provide both multiplicands.</p>")
#     print("</body></html>")
