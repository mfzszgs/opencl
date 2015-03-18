LBS=-lOpenCL -L/usr/local/cda/lib64/
OBJS=src/main.o
CPPFLAGS=-I/usr/local/cuda/include

all:${OBJS}
	g++ -o $@ $< ${LBS}

${OBJS}:src/*.h 

.PHONY:clean
clean:
	rm -f main ${OBJS}
