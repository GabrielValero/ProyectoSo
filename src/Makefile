

OBJS = main.o Cola.o
TARGET = program
CC = gcc

all: $(TARGET) clean

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c %.h

clean: 
	rm -rf $(OBJS)