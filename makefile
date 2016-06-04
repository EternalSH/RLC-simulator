ASM=nasm
CXX=gcc
LD=gcc

ASMFLAGS=-f elf64
CXXFLAGS=-std=gnu99 -c -Wall -Werror -pedantic -Wextra
LDFLAGS=-L/usr/local/lib -lallegro_primitives -lallegro_ttf -lallegro_font -lallegro

.PHONY: clean

all: simulation

run: simulation
	./bin/simulation

obj/sim.o: src/sim.asm
	$(ASM) $(ASMFLAGS) -o obj/sim.o src/sim.asm

obj/main.o: src/main.c
	$(CXX) $(CXXFLAGS) -o obj/main.o src/main.c

simulation: obj/sim.o obj/main.o
	$(LD) $(LDFLAGS) -o bin/simulation obj/sim.o obj/main.o

clean:	
	rm bin/* obj/*
