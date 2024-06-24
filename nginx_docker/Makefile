IMAGE_NAME = nginx_img
CONTAINER_NAME = nginx_my
DOCKERFILE_PATH = .
PORT = 4247

all: build run

build :
	docker build -t ${IMAGE_NAME} ${DOCKERFILE_PATH}

run:
	docker run -d --name ${CONTAINER_NAME} -p ${PORT}:${PORT} ${IMAGE_NAME}

exec:
	docker exec -it ${CONTAINER_NAME} /bin/bash

stop:
	docker stop ${CONTAINER_NAME}

rm:
	docker rm ${CONTAINER_NAME}

rmi:
	docker rmi ${IMAGE_NAME}

clean: rm rmi

.PHONY: all run stop rm rmi clean
