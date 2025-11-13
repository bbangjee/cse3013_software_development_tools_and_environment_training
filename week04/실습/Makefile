CC = gcc
TARGET = animal_exe
OBJECTS = dog.o blackcow.o turtle.o main.o

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

$(OBJECTS) : animal.h

.PHONY : clean

clean : 
	rm $(TARGET) $(OBJECTS)