#!/bin/sh
# This script creates an image meant to run on the car, it uses the Dockerfile.armhf to compile code meant for the
# armhf architecture.

docker build -t group7/carcode -f Dockerfile.armhf .
docker save group7/carcode > crosscompiledcarcode.tar
