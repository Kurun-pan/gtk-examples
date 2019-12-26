CCOMP := clang

all: hello_world_old hello_world

hello_world_old: hello_world_old.c 
	${CCOMP} -o hello_world_old hello_world_old.c `pkg-config --cflags --libs gtk+-3.0`

hello_world: hello_world.c 
	${CCOMP} -o hello_world hello_world.c `pkg-config --cflags --libs gtk+-3.0`	

clean:
	rm -f hello_world_old hello_world *.o
