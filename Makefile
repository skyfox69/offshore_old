MAKE=make
SHELL=/bin/sh

default: offshore

CC = g++

CFLAGS = -g -O0 -std=c++14 -I. -pipe -Wp,-fexceptions -fstack-protector --param=ssp-buffer-size=4  -m64 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -fno-strict-aliasing -fwrapv -fPIC -static-libgcc -fno-omit-frame-pointer -fno-strict-aliasing -DMY_PTHREAD_FASTMUTEX=1

#CFLAGS = -g -O -std=c++11 -pipe -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4  -m64 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -fno-strict-aliasing -fwrapv -fPIC  -fPIC -static-libgcc -fno-omit-frame-pointer -fno-strict-aliasing -DMY_PTHREAD_FASTMUTEX=1

LIBS =

LDFLAGS = -g -lpthread -lz -lm -lrt -ldl -lcurl

PRG = offshore
SRC = main.cpp \
      options.cpp \
      urlloader.cpp \
      urlcrawler.cpp \
      urllink.cpp \

OBJ = ${SRC:%.cpp=%.o}

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ ${@:.o=.cpp}

${PRG}: ${OBJ}
	$(CC) $(LDFLAGS) -o ${PRG} ${OBJ} $(LIBS)

clean:
	rm -f ${PRG}
	rm -f ${OBJ}

rebuild: clean offshore

