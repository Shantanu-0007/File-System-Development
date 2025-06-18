CC=gcc
CFLAGS=`pkg-config fuse3 --cflags` -Wall
LDFLAGS=`pkg-config fuse3 --libs`
SRC=src/main.c
OUT=build/myfs

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f build/myfs
