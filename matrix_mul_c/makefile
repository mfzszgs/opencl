LBS= -framework OpenCL
OBJ=src/main.o src/cl_create.o src/cl_matrix.o
CFLAGS= -framework OpenCL

all:${OBJ}
	gcc -o $@ ${OBJ} ${LBS}
clean:
	-rm all ${OBJ}
