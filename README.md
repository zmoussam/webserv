# Webserv

A recreation of Nginx and Apache webservers in C++

## TODO

- [ ] Add I/O Multiplexing
- [x] HTTP Request Parsing
- [ ] HTTP Response Generation
- [ ] POST, PUT, DELETE, HEAD, OPTIONS, TRACE, CONNECT methods support
- [ ] Parse and handle configuration file
- [ ] Parse and handle CGI scripts
- [ ] Parse and handle SSL certificates
- [ ] Error handling
- [ ] File I/O
- [ ] Percent encoding and decoding for URLs
- [ ] ...

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
- **cfg**: Used when modifying server configurations or settings.


## Resources

- https://datatracker.ietf.org/doc/html/rfc2616#section-1
- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
- https://www3.ntu.edu.sg/home/ehchua/programming/webprogramming/http_basics.html
- https://beej.us/guide/bgnet/html/split/
- https://notes.shichao.io/unp/
- https://developer.mozilla.org/en-US/
