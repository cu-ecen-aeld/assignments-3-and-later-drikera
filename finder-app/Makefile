
# vim: set noexpandtab:

#CROSS_COMPILE:=aarch64-none-linux-gnu-
#CROSS_COMPILE:=

all: writer

writer: writer.c
	$(CC) $(CFLAGS) writer.c -o writer $(LDFLAGS)

.PHONY: clean

clean:
	rm -f writer
	
