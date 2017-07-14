CC = gcc
CFLAGS = -std=c99
OBJS = q2.o

all: ../build/Q2

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

../build/Q2: $(OBJS)
	-mkdir -p ../build
	$(CC) -o $@ $^

.PHONY: clean
clean:
	@echo "Clean..."
	-rm *.o 
	-rm ../build/Q2
