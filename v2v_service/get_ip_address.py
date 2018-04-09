#!/usr/bin/env python2

import socket

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(("8.8.8.8", 80))
ip = s.getsockname()[0]
s.close()

with open("ip.txt", "w") as ip_file:
    ip_file.write(ip)

