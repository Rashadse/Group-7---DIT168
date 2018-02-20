# Author: Rashad Kamsheh

echo "Docker"

docker build -t myrepository/mydockerimage -f Dockerfile .
docker run --rm -ti --net=host myrepository/mydockerimage
