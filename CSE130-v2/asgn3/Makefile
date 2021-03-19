CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -O2
TARGET = loadbalancer
FILES = loadbalancer.c Makefile README.md WRITEUP.pdf DESIGN.pdf

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $(CFLAGS) -pthread -o $(TARGET) $(TARGET).o

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c

clean: 
	rm $(TARGET).o
	
spotless: clean
	rm $(TARGET)
	
gdb:
	$(CC) $(CFLAGS) -g -pthread -o $(TARGET) $(TARGET).c
	
git:
	git add $(FILES)
	git commit -a -m "$m"
	git push --all
