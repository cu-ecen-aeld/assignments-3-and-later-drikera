
# vim: set noexpandtab:

CROSS_COMPILE:=aarch64-none-linux-gnu-

all: writer

writer: writer.c
	$(CROSS_COMPILE)gcc writer.c -o writer

.PHONY: clean

clean:
	rm -f writer
	
