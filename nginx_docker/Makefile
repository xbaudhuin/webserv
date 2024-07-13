IMAGE_NAME = nginx_img
CONTAINER_NAME = nginx_my
DOCKERFILE_PATH = .
PORT1 = 4247
PORT2 = 4248

all: build run

build :
	docker build -t ${IMAGE_NAME} ${DOCKERFILE_PATH}

run:
	docker run -d --name ${CONTAINER_NAME} -p ${PORT1}:${PORT1} -p ${PORT2}:${PORT2} ${IMAGE_NAME}

exec:
	docker exec -it ${CONTAINER_NAME} /bin/bash

stop:
	docker stop ${CONTAINER_NAME}

rm:
	docker rm ${CONTAINER_NAME}

rmi:
	docker rmi ${IMAGE_NAME}

clean: rm rmi

re: stop clean all

.PHONY: all run stop rm rmi clean
