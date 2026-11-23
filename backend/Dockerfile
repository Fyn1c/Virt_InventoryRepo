FROM gcc:latest AS build

RUN apt-get update && apt-get install -y cmake

ADD . /app

WORKDIR /app

RUN cmake -B build && cmake --build build 

FROM ubuntu:latest 

RUN groupadd -r sample && useradd -r -g sample sample
USER sample

WORKDIR /app
COPY --from=build /app/build/backend-service .

ENTRYPOINT ["./backend-service"]