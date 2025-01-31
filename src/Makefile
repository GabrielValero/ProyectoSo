

OBJS = main.o Queue.o Utils.o
TARGET = duplicados
CC = gcc

all: $(TARGET) clean

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c %.h

clean: 
	rm -rf $(OBJS)