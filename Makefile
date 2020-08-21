CC=gcc
FLAGS=-g -W

BIN_DIR=./bin
SRC_DIR=./src
OBJ_DIR=./obj

LIBS= -lwsock32

SRCS= $(wildcard $(SRC_DIR)/*.c)
OBJS= $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

FILENAME=Sock

all: clean main

main: $(OBJS)
	$(CC) $(FLAGS) $^ -o $(BIN_DIR)/$(FILENAME).exe $(LIBS)

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	make -C ./obj/ -f Clean clean