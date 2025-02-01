

OBJS = main.o Queue.o Utils.o Semaforo.o 
TARGET = duplicados
CC = gcc

all: $(TARGET) clean

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -L./md5-lib -lmd5 -o $(TARGET)

%.o: %.c %.h

clean: 
	rm -rf $(OBJS)