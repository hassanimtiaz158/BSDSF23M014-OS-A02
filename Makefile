CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/lsv1.0.0.c
OBJ = obj/lsv1.0.0.o
BIN = bin/ls

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)
