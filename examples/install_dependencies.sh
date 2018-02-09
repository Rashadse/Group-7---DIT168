#!/bin/sh
sudo apt-add-repository ppa:chrberger/libcluon
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install git cmake build-essential libcluon socat
