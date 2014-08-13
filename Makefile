EXEC=simulation
SOURCES=run_simulation.c parse_stl.c

C_FLAGS=-Wall
LD_FLAGS=-lm

all: 
	gcc ${SOURCES} ${C_FLAGS} -o ${EXEC} ${LD_FLAGS}

clean:
	rm ${EXEC} 
