all:
	make file_image.bin

clean:
	rm -f *.bin

%.bin: %.c
	h5cc "$<" -o "$@" -O
