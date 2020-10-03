#----------------------------------------------------------------
#
#  4190.308 Computer Architecture (Fall 2020)
#
#  Project #1: Compressing Data with Huffman Coding
#
#  September 9, 2020
#
#  Injae Kang (abcinje@snu.ac.kr)
#  Sunmin Jeong (sunnyday0208@snu.ac.kr)
#  Systems Software & Architecture Laboratory
#  Dept. of Computer Science and Engineering
#  Seoul National University
#
#----------------------------------------------------------------

TARGET	= pa1
SRCS	= pa1.c pa1-main.c pa1-test.c
CC	= gcc
CFLAGS	= -g -O2 -Wall
OBJS	= $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS)
