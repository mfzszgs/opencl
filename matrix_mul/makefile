LBS=-lOpenCL -L/usr/local/cuda/lib64/
OBJ=src/MatrixMul.o
CPPFLAGS=-I/usr/local/cuda/include/

all:${OBJ}
	g++ -o $@ ${OBJ} ${LBS}
clean:
	-rm all ${OBJ}
