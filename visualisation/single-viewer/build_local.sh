#!/bin/sh
# Author: Rashad Kamsheh
# This script builds an image for the current platform from which it is being build. Building using this script on the
# VM and then transfering the built image to the car will not work.


docker build -t group7/singleviewer -f Dockerfile.amd64 .
docker run --rm --net=host -p 8080:8080 group7/singleviewer --cid=182

