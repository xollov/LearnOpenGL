
CC = gcc
FLAGS =  -Wall -std=c11
SRCS = main.c glad.c Graphics/shader.c
LIBS = -lglfw -lGL -lm -lassimp
TARGET = w.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(FLAGS) -o $(TARGET) $(SRCS) $(LIBS)

debug:
	$(CC) $(FLAGS) -o $(TARGET) $(SRCS) $(LIBS) -g
