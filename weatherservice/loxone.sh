#!/bin/bash

server="ip-mini-server"
port="80"

user=admin
password="admin"

curl -u "${user}:${password}" "http://${server}:${port}/dev/fsget/log/def.log" | less
