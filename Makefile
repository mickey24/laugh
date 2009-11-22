program = laugh
CC = gcc
CFLAGS = -O2 -std=gnu99 -I/opt/local/include/opencv -L/opt/local/lib -lcxcore -lcv -lml -lhighgui -lcvaux

$(program): laugh.c
	$(CC) -o $(program) $(CFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(program)
