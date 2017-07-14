CC = gcc
CFLAGS = -std=c99
OBJS = q1.o

all: ../build/Q1

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

../build/Q1: $(OBJS)
	-mkdir -p ../build
	$(CC) -o $@ $^

.PHONY: clean
clean:
	@echo "Clean..."
	-rm *.o 
	-rm ../build/Q1
