TOP := $(shell pwd)
PROJECT_BASE := $(TOP)

INCLUDE_DIR = -I$(PROJECT_BASE)/include
INCLUDE_DIR += -I/usr/include/X11
INCLUDE_DIR += -I/usr/include/freetype2


LIB_DIR = -L$(PROJECT_BASE)/lib
LIB_DIR += -lX11
LIB_DIR += -lXft





.SUFFIXES : .c .o

CC = gcc

INC = $(INCLUDE_DIR)
LIBS = $(LIB_DIR)
CFLAGS = -g $(INC) 
LDLFAGS = -L

OBJS = main.o
SRCS = $(OBJS:.o=.c)

TARGET = tp

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

dep :
	gccmakedep $(INC) $(SRCS)

clean :
	rm -rf $(OBJS) $(TARGET) core

new : 
	$(MAKE) clean 
	$(MAKE)
