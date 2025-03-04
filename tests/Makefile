KERNEL = ../kernel/dataset.c \
	../kernel/transform.c \
	../kernel/utils.c \

all: build

build:
	$(CC) -g main.c $(KERNEL) -pthread -lrt -lm -lsubunit -o a.out

clean:
	rm a.out