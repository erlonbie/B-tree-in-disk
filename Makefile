run-all: build run hash-allocate compile-upload compile-findrec compile-seek1 access-container

.PHONY: build
build:
	@docker-compose build

.PHONY: run
run:
	@docker-compose up -d

.PHONY: hash-allocate
hash-allocate:
	@docker exec -it tp2_aldemir_erlon_glenn fallocate -l 4G hash.bin

.PHONY: access-container
access-container:
	@docker exec -it tp2_aldemir_erlon_glenn bash

.PHONY: compile-upload
compile-upload:
	@docker exec tp2_aldemir_erlon_glenn g++ upload.cpp -o upload

.PHONY: compile-findrec
compile-findrec:
	@docker exec tp2_aldemir_erlon_glenn g++ findrec.cpp -o findrec

.PHONY: compile-seek1
compile-seek1:
	@docker exec tp2_aldemir_erlon_glenn g++ seek1.cpp -o seek1

.PHONY: end
end:
	@docker-compose down
