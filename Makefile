all:
	make test.bin

clean:
	rm -f *.o *.bin

%.bin: %.c
	h5cc "$<" -o "$@" -O
