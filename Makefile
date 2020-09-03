TARGET = minecurses
INSTALL_PATH = /usr/local/bin

SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=%.o)

CC = gcc
CPPFLAGS += -Iinclude -pedantic
CFLAGS += -Wall -std=c99 -O3
LDFLAGS += -Llib
LDLIBS += -lncurses -lSDL2 -lSDL2_mixer -lpthread

CP=cp
MOVE = mv
MKDIR_P = mkdir -p
RM_DIR=rm -rf

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

install: $(TARGET)
	$(CP) $(TARGET) $(INSTALL_PATH)

clean:
	$(RM) $(OBJ) $(TARGET)
