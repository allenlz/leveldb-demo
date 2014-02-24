
.PHONY : all clean output test deps

CC = clang++
SHELL = /bin/sh

INCLUDES = \
		   -Ideps/leveldb/include

LDFLAGS =  -g \
		   deps/leveldb/libleveldb.a \
		   -lpthread -lz -lm
		   #-L../leveldb -lleveldb \
		   #-Wl,-rpath ../leveldb \

CPPFLAGS = $(INCLUDES) $(VER_INFO) -g -pipe \
		   -D__STDC_LIMIT_MACROS\
		   -W -Wall -Wno-unused-parameter -Wreturn-type \
		   -Wtrigraphs -Wformat -Wparentheses -Wpointer-arith -Werror -Winline

SOURCES = $(wildcard *.cpp)
#OBJS := $(patsubst %.cpp, %.o, $(SOURCES))

BINS = simple_demo expired_time_demo repair_if_failed

all : deps $(BINS)

deps :
	make -C deps/leveldb

*.o : deps

clean :
	rm -rf $(OBJS) *.o
	rm -rf $(BINS)

simple_demo : simple_demo.o
	$(CC) -o $@ $< $(LDFLAGS)

expired_time_demo : expired_time_demo.o
	$(CC) -o $@ $< $(LDFLAGS)

repair_if_failed : repair_if_failed.o
	$(CC) -o $@ $< $(LDFLAGS)

%.o : %.cpp
	$(CC) -c $^ $(CPPFLAGS)

