FROM gcc:10

ENV DOCKER_RUN 1
ENV REFRESHED_AT 2021-04-22

RUN mkdir /home/project
WORKDIR /home/project

RUN apt-get update && apt-get install -y cmake curl libsodium-dev cpputest lcov

