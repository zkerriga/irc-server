#!/bin/zsh

pass=PASS
ourPassword=MySecret

server=SERVER
ourServerName=irc2.example2.net
ourServerInfo=experiment

echo -n "$pass $ourPassword 0210-IRC+ ngircd|0.7.5:\r\n$server $ourServerName 1 :$ourServerInfo\r\n" | nc localhost 6667 > result.txt