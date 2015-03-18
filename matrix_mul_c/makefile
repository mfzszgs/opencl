LBS=-lOpenCL -L/usr/local/cuda/lib64/
OBJ=src/main.o
CFLAGS=-I/usr/local/cuda/include/ -std=c99 

all:${OBJ}
	gcc -o $@ ${OBJ} ${LBS}
${OBJ}:src/cl_create.h src/matrix_mul.h
clean:
	-rm all ${OBJ}
