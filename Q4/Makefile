CC = gcc
CFLAGS = -std=gnu99 -DDETECT_ERROR
OBJS = q4.o

all: ../build/Q4

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

../build/Q4: $(OBJS)
	-mkdir -p ../build
	$(CC) -o $@ $^

.PHONY: clean
clean:
	@echo "Clean..."
	-rm *.o 
	-rm ../build/Q4
