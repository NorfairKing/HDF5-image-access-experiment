all:
	make example.bin

clean:
	rm -f *.o *.bin

%.bin: %.c
	h5cc "$<" -o "$@" -O
