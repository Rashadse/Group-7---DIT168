#!/bin/sh
sudo apt-add-repository ppa:chrberger/libcluon
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install cmake build-essential libcluon socat docker-ce
sudo apt install curl
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add
sudo groupadd docker
sudo usermod -aG docker $USER
