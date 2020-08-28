CC= gcc
CFLAGS=-O3 -no-pie
LINKS=-lm

.PHONY: all
all: encode decode
decode: decode.c decode.S
		$(CC) $(CFLAGS) -o $@ $^ $(LINKS)
encode: encode.c encode.S
	$(CC) $(CFLAGS) -o $@ $^ $(LINKS)
.PHONY: clean

clean:
	rm -f encode
	rm -f decode
