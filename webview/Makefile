CC := clang

CFLAGS = `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0` -Wall
LIBS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0`

src = main.c
obj = webview_sample
targets = webview_sample

all: $(targets)

webview_sample: $(src)
	$(CC) -o $(obj) $(src) $(CFLAGS) $(LIBS) 

clean:
	rm -f $(targets) *.o *.*~
