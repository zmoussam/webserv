
for port in range(8000, 8100):
	config = """server {
		listen: "%d"
		host: "127.0.0.1"
		server_name: "localhost"
		root: "/Volumes/BIGBOY/root"
		index: "index.html"
		client_body_size: "60000000"
		allow_methods: "GET","POST","DELETE"
		upload_path: "/upload"
		error_page: "404","erros/404.html"
		autoindex: "on"
		location "/cgi" {
			allow_methods: "GET","POST","DELETE"
			root: "www/bruh"
			index: "index.html"
			allow_methods: "GET","POST","DELETE"
			upload_path: "/upload"
		}
		location "/idklol" {
			allow_methods: "GET","POST","DELETE"
			root: "/tmp/testo"
			upload_path: "/upload"
			error_page: "404","/Users/aarbaoui/Documents/1337/webserv/www2/index.html"
		}
		location "\.py" {
			allow_methods: "GET","POST"
			root: "www/"
			compiler: "/usr/local/bin/python3.9"
		}
		location "\.rb" {
			allow_methods: "GET","POST"
			root: "www/"
			compiler: "/usr/bin/ruby"
		}
		location "/rick" {
			
			return: "https://www.youtube.com/watch?v=dQw4w9WgXcQ"
		}
	}
""" % port
	with open("test.conf", "a") as f:
		f.write(config)