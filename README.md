# irc-server

:computer::left_right_arrow::computer: Go back to the past and raise the IRC network!

### Objective of program:
The purpose of writing the program was to create **server-server** and **client-server** interactions within the IRC-protocols (RFC [1459](https://tools.ietf.org/html/rfc1459), [2810](https://tools.ietf.org/html/rfc2810), [2811](https://tools.ietf.org/html/rfc2811), [2812](https://tools.ietf.org/html/rfc2812), [2813](https://tools.ietf.org/html/rfc2813), and [7194](https://tools.ietf.org/html/rfc7194)), using only C++98, STL, algorithm and cryptographic libraries.

### Prepare:
1. Clone the repository with the `--recursive` flag (this will allow you to immediately pull up the used libraries)
2. Compile the program: `make`
3. Specify the *KeyFile* and *CrtFile* parameters in the *ircserv.conf* configuration file in the *SSL* section (these files must be generated and contain certificates) or use `make certs` to generate the files automatically.

### Usage:
Start the server!
```
./ircserv [host:port_network:password_network] <port> <password>
```
To get detailed information about the parameters, just use `./ircserv`.

Now you can connect to the server via a third-party client (for example: [irssi](https://irssi.org/)), via a third-party server (for example: [ngircd](https://ngircd.barton.de/)), by running current server on a different port (extended startup arguments), or simply using the `nc -c localhost <port>`.

To connect as a server:
```
PASS pass 0210-IRC+ ngIRCd| P
SERVER test.net :info about server
```
To connect as a client:
```
PASS pass
NICK MySuperNick
USER MyUsername ModeOrServer MyInfo :My real Name
```

### Commands:
The server is able to process the following list of commands:

*PASS*, *MOTD*, *STATS*, *NOTICE*, *NAMES*, *INVITE*, *PRIVMSG*, *TRACE*, *LINKS*, *INFO*, *LIST*, *KICK*, *ADMIN*, *TOPIC*, *PART*, *MODE*, *VERSION*, *NJOIN*, *NICK*, *OPER*, *PONG*, *SQUIT*, *QUIT*, *SERVER*, *CONNECT*, *TIME*, *USER*.

Some commands don't implement all the functionality described in the protocols, but provide the main work.
