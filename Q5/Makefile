CC = gcc
CFLAGS = -std=c99
OBJS = fork.o

all: ../build/fork

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

../build/fork: $(OBJS)
	-mkdir -p ../build
	$(CC) -o $@ $^

.PHONY: clean
clean:
	@echo "Clean..."
	-rm *.o 
	-rm ../build/fork
