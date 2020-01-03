CC := clang

CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall
LIBS = `pkg-config --libs gtk+-3.0 epoxy`

src = main.c draw.c resources.c
obj = opengl_draw
targets = opengl_draw

all: $(targets)

opengl_draw: $(src)
	$(CC) -o $(obj) $(src) $(CFLAGS) $(LIBS) 

clean:
	rm -f $(targets) *.o *.*~
