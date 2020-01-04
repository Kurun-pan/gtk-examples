CC := clang

CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall
LIBS = `pkg-config --libs gtk+-3.0`

src = main.c
obj = g_thread_sample
targets = g_thread_sample

all: $(targets)

g_thread_sample: $(src)
	$(CC) -o $(obj) $(src) $(CFLAGS) $(LIBS) 

clean:
	rm -f $(targets) *.o *.*~
