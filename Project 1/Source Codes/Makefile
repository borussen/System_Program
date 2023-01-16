CC = gcc
TARGET = main
OBJECTS = sfp.o main.o

$(TARGET) : main.o sfp.o
	$(CC) -o $(TARGET) $(OBJECTS)

main.o : main.c
	$(CC) -c -o main.o main.c

sfp.o : sfp.c
	$(CC) -c -o sfp.o sfp.c

clean :
	rm *.o $(TARGET)
