#!/bin/sh
# Author: Rashad Kamsheh
# This script builds an image for the current platform from which it is being build. Building using this script on the
# VM and then transfering the built image to the car will not work.

./get_ip_address.py  # IMPORTANT! Gets the current IP address and passes to the container via a generated .txt
docker build -t group7/carcode -f Dockerfile .
