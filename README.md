# Webserv

A recreation of Nginx and Apache webservers in C++

## Commit Message Convention

We use a commit message convention to ensure clear and meaningful communication about the changes made in each commit. When making commits, please follow the naming convention below:

- **add**: Used when adding a new file or files to the server.
- **rm**: Used when removing a file or files from the server.
- **edit**: Used when making changes to an existing file or files in the server that do not fall under any of the other commit types.
- **feat**: Used when adding a new feature or functionality to the server.
- **fix**: Used when fixing a bug or resolving an issue in the server.
- **ref**: Used when making code changes that improve the structure, design, or organization of the server without changing its behavior.
- **chore**: Used for routine tasks, maintenance, or general housekeeping, such as updating dependencies, cleaning up code, or modifying build scripts.
- **docs**: Used for changes or additions to documentation, comments, or code examples.
- **test**: Used when adding or modifying test cases or related code to improve test coverage.
- **style**: Used for code style changes that do not affect the server's behavior, such as formatting, indentation, or whitespace modifications.
- **perf**: Used when making performance improvements or optimizations to the server.
- **rev**: Used when reverting a previous commit or a series of commits.
- **merge**: Used for merge commits, indicating the integration of changes from one branch into another.
- **config**: Used when modifying server configurations or settings.

## Folder Structure

The project follows the following folder structure:
```
webserv/
├── src/
│ ├── main.cpp
│ ├── Server/
│ │ ├── Server.hpp
│ │ └── Server.cpp
│ ├── Request/
│ │ ├── Request.hpp
│ │ └── Request.cpp
│ ├── Response/
│ │ ├── Response.hpp
│ │ └── Response.cpp
│ ├── HTTP/
│ │ ├── HTTP.hpp
│ │ └── HTTP.cpp
│ ├── Utils/
│ │ ├── Utils.hpp
│ │ └── Utils.cpp
│ └── Configuration/
│ ├── Configuration.hpp
│ └── Configuration.cpp
├── include/
│ ├── Server/
│ │ └── Server.hpp
│ ├── Request/
│ │ └── Request.hpp
│ ├── Response/
│ │ └── Response.hpp
│ ├── HTTP/
│ │ └── HTTP.hpp
│ ├── Utils/
│ │ └── Utils.hpp
│ └── Configuration/
│ └── Configuration.hpp
```

- `src/`: This directory contains the source files of the project.
  - `main.cpp`: The main entry point of the server.
  - `Server/`: Contains the server-related classes and implementation.
  - `Request/`: Contains the classes and implementation related to handling HTTP requests.
  - `Response/`: Contains the classes and implementation related to generating HTTP responses.
  - `HTTP/`: Contains the HTTP-related classes and implementation.
  - `Utils/`: Contains utility classes and functions used throughout the project.
  - `Configuration/`: Contains classes and implementation for server configuration.

- `include/`: This directory contains the header files of the project. It mirrors the structure of the `src/` directory.

Feel free to modify the folder structure or add more folders/files as needed for your project. Make sure to keep the folder structure organized and consistent to enhance code maintainability and readability.

## Resources

- https://datatracker.ietf.org/doc/html/rfc2616#section-1
- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
- https://www3.ntu.edu.sg/home/ehchua/programming/webprogramming/http_basics.html
- https://beej.us/guide/bgnet/html/split/
- https://notes.shichao.io/unp/
- https://developer.mozilla.org/en-US/
