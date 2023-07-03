import socket
import os
from multiprocessing import Process

HOST, PORT = '127.0.0.1', 8000

def handle_request(client_connection):
    # Get the request data
    request_data = client_connection.recv(1024).decode('utf-8')
    request_lines = request_data.splitlines()
    print("Request from: " + f"{client_connection.getpeername()[0]}:{client_connection.getpeername()[1]}" + " " + request_lines[0])
    if len(request_lines) < 1:
        client_connection.close()
        return

    get = request_lines[0].split()[1]

    if get == "/":
        get = "/index.html"

    try:
        type = get_type(get)
        code = "200"
        file = open("." + get, "rb")  # Open the file in binary mode! Important!
        data = file.read()
        file.close()
    except Exception as e:
        code = "404"
        data = f"<html><body><center><h1>Error 404: File not found</h1></center><br><center><p>{e}<\p></center></body></html>"
        type = "text/html"

    # Send HTTP response
    http_response = b'''HTTP/1.1 %s OK\nServer: webserv/1.0\nContent-Length: %s\nContent-Type: %s\n\n%s''' % (
        code.encode('utf-8'), str(len(data)).encode('utf-8'), type.encode('utf-8'), data)
    client_connection.sendall(http_response)
    client_connection.close()

def get_type(file):
    if file.endswith(".html"):
        return "text/html"
    elif file.endswith(".jpg"):
        return "image/jpg"
    elif file.endswith(".png"):
        return "image/png"
    elif file.endswith(".gif"):
        return "image/gif"
    elif file.endswith(".css"):
        return "text/css"
    elif file.endswith(".js"):
        return "application/javascript"
    elif file.endswith(".ico"):
        return "image/x-icon"
    else:
        return "text/plain"

if __name__ == '__main__':
    # Create a socket object
    listen_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Allow to reuse the same address
    listen_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Bind the socket to the port
    listen_socket.bind((HOST, PORT))

    # Listen for incoming connections
    listen_socket.listen(500)  # Maximum number of queued connections

    print('Serving HTTP on port http://%s:%s ...' % (HOST, PORT))
    os.chdir("www")

    while True:
        # Wait for client connections
        client_connection, client_address = listen_socket.accept()

        # Create a new process to handle the client request
        process = Process(target=handle_request, args=(client_connection,))
        process.start()

        # Close the connection in the parent process
        client_connection.close()
