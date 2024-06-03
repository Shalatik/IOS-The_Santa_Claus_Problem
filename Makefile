# Projekt: VUT FIT IOS proj2
# Autor: Simona Ceskova xcesko00
# Datum: 02.05.2021

CC=gcc
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic

proj2: proj2.c
	$(CC) $(CFLAGS) -pthread proj2.c -o proj2 -lrt
