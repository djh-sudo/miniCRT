BUILD_PATH = ./build
GCC = gcc
CFLAGS = -fno-builtin -nostdlib -fno-stack-protector -ggdb -Wall -O0 -m32

entry: Makefile entry.c
	$(GCC) -c $(CFLAGS) entry.c -o $(BUILD_PATH)/entry.o

malloc: Makefile malloc.c
	$(GCC) -c $(CFLAGS) malloc.c -o $(BUILD_PATH)/malloc.o 

stdio: Makefile stdio.c
	$(GCC) -c $(CFLAGS) stdio.c -o $(BUILD_PATH)/stdio.o

# add in day03
string: Makefile string.c
	$(GCC) -c $(CFLAGS) string.c -o $(BUILD_PATH)/string.o

main: Makefile main.c
	$(GCC) -c $(CFLAGS) main.c -o $(BUILD_PATH)/main.o

obj: Makefile
	make entry
	make malloc
	make stdio
	make string

link: Makefile
	ar -rs $(BUILD_PATH)/minicrt.a \
	$(BUILD_PATH)/entry.o $(BUILD_PATH)/malloc.o $(BUILD_PATH)/stdio.o $(BUILD_PATH)/string.o

update: Makefile
	make main
	ld -static -e mini_crt_entry \
	$(BUILD_PATH)/entry.o $(BUILD_PATH)/main.o $(BUILD_PATH)/minicrt.a \
	-o $(BUILD_PATH)/main

run: Makefile
	make clear
	mkdir $(BUILD_PATH)
	make obj
	make link
	make update
	
clear:
	rm -rf build/