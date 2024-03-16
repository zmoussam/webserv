# Webserv

## Introduction
Webserv is a project focused on implementing a custom HTTP server in C++ 98. The main objective of this project is to gain a deep understanding of the HTTP protocol and server-side programming concepts. By creating our own HTTP server, we delve into the intricacies of handling HTTP requests, serving web pages, managing file uploads, and more.

## Features Implemented
- **HTTP Server**: The core functionality of the project involves creating an HTTP server that can handle incoming HTTP requests and respond appropriately.
- **Configuration File**: The server accepts a configuration file as an argument, allowing users to customize settings such as port, host, server names, error pages, and more.
- **Non-blocking I/O**: The server is designed to be non-blocking, ensuring that clients can be handled efficiently without blocking other operations.
- **Single Poll for I/O**: Utilizes a single `poll()` (or equivalent) function to handle all I/O operations, including listening for incoming connections and processing requests.
- **HTTP Methods**: Supports essential HTTP methods such as GET, POST, and DELETE.
- **Static Website Serving**: Capable of serving fully static websites, delivering HTML documents, images, stylesheets, and scripts.
- **File Uploads**: Allows clients to upload files to the server.
- **Error Handling**: Provides default error pages in case no specific error pages are configured.
- **CGI Execution**: Implements CGI execution for certain file extensions (e.g., .php) to enable dynamic content generation.
- **Bonus Features**: Additional features like support for cookies and session management, as well as handling multiple CGI instances.

## Usage
1. **Compilation**: Use the provided Makefile to compile the project. Run the following command:
```
  make
```
This will compile all the necessary source files and generate the executable `webserv`.

2. **Execution**: Once compiled, you can run the HTTP server with the following command:
```
  ./webserv [configuration_file]
```
Replace `[configuration_file]` with the path to your desired configuration file.

## Evaluation
This project was evaluated based on adherence to project requirements, implementation of mandatory features, adherence to coding standards, and overall functionality. The code was written in C++ 98, following strict guidelines, and compiled with necessary flags.

## Acknowledgments
I would like to express my gratitude to [1337 coding school](https://1337.ma/) for providing the opportunity to work on this project and enhance my skills in server-side programming and HTTP protocol handling.

