BUILD_PATH = ./build
GCC = gcc
CFLAGS = -fno-builtin -nostdlib -fno-stack-protector

entry: Makefile entry.c
	$(GCC) -c $(CFLAGS) entry.c -o $(BUILD_PATH)/entry.o

malloc: Makefile malloc.c
	$(GCC) -c $(CFLAGS) malloc.c -o $(BUILD_PATH)/malloc.o 

# add in day02
stdio: Makefile stdio.c
	$(GCC) -c $(CFLAGS) stdio.c -o $(BUILD_PATH)/stdio.o

obj: Makefile
	make entry
	make malloc
	make stdio

link: Makefile
	ar -rs $(BUILD_PATH)/minicrt.a \
	$(BUILD_PATH)/entry.o $(BUILD_PATH)/malloc.o $(BUILD_PATH)/stdio.o

run: Makefile
	mkdir $(BUILD_PATH)
	make obj
	make link
	gcc -c -ggdb $(CFLAGS) main.c -o $(BUILD_PATH)/main.o
	ld -static -e mini_crt_entry \
	$(BUILD_PATH)/entry.o $(BUILD_PATH)/main.o $(BUILD_PATH)/minicrt.a \
	-o $(BUILD_PATH)/main

clear:
	rm -rf build/