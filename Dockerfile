FROM python:3.7

WORKDIR /app

COPY . .

RUN apt -y update && apt install -y cmake g++ make git

RUN fallocate -l 4G hash.bin

WORKDIR /app

CMD ["bash"]
