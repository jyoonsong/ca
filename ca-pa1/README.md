# 4190.308 Computer Architecture (Fall 2020)
# Project #1: Compressing Data with Huffman Coding
### Due: 11:59PM, September 20 (Sunday)


## Introduction

This project demonstrates how we can compress data using a simplified Huffman coding. The purpose of this project is to make you familiar with the binary representations of strings/integers and the logical operations supported in the C programming language. Another goal is to make your Linux or MacOS development environment ready and to get familiar with our project submission server. 


## A Simplified Huffman coding

### Huffman coding

A Huffman code is a particular type of optimal prefix code that is commonly used for loseless data compression. The output from Huffman's algorithm can be viewed as a variable-length code table for encoding a source symbol. Basically, the Huffman's algorithm measures the frequency of each symbol and uses this information to assign fewer bits to more common symbols. For more information on Huffman coding, please refer to https://en.wikipedia.org/wiki/Huffman_coding.

### Our simplified Huffman coding

In the original Huffman's algorithm, the actual encoded values are determined dynamically depending on the frequencies of source symbols. To make the problem simpler, we just use a static encoding table. Our simplified Huffman coding works as follows:

1. Each symbol consists of 4 bits. Therefore, the input data is divided into 4-bit symbols and there will be 16 different symbols from `0000` to `1111`.
2. For each 4-bit symbol, we count the number of occurrences in the input data.
3. Now we sort the symbols based on the total number of occurrences and assign the code statically as shown in the following table. 

| Symbol Rank |   Code   | Description       |
|:----:|:--------:|--------------------------|
|  0   |   000    | the most frequent symbol                      |
|  1   |   001    | the next most frequent symbol                 |
|  2   |   010    |                                               |
|  3   |   011    |                                               |
|  4   |   1000   |       ...                                     |
|  5   |   1001   |                                               |
|  6   |   1010   |                                               |
|  7   |   1011   | the 8th most frequent symbol                  |
|  8   |   11000  | the smallest symbol value among the rest      |
|  9   |   11001  | the next smallest symbol value among the rest |
|  10  |   11010  |                                               |
|  11  |   11011  |                                               |
|  12  |   11100  |         ...                                   |
|  13  |   11101  |                                               |
|  14  |   11110  |                                               |
|  15  |   11111  | the largest symbol value among the rest       |

We keep the symbol values of the most frequent 8 symbols in the header of the output for decompression (shown below). If the number of occurrences is same, we give a higher rank to a smaller symbol value. For the rest of them (i.e., the least frequent 8 symbols), however, we can see that the lengths of the encoded values are all the same. In order to reduce the output size, we don't keep the symbol values in the header. Instead, we arrange them according to the actual symbol value by interpreting them as unsigned integers (see the example below).

4. The output has the following format.

| _R_ (Rank table) | _E_ (End info) | _D_ (Encoded data) | Padding   |
|------------------|----------------|--------------------|-----------|
| (32 bits)        | (4 bits)       | (_n_ bits)         | (0~7 bits)|


The __Rank table__ records the most frequent 8 symbols in the order of their ranks. The __End info__ tells the number of bits padded in the last byte. The __Encoded data__ area has a sequence of encoded values that corresponds to the input data. The final output should be aligned to __bytes__. If the total number of bits for the header and the encoded data (i.e., 32 (_R_) + 4 (_E_) + _n_ (_D_)) is not a multiple of 8, you should pad 0's in the last byte so that the total number of bits becomes a multiple of 8.

### Example

Let's see an example. Assume that we want to compress the string `The quick brown fox jumps over the lazy dog.` using the simplified Huffman coding. Each character in the text string in C is represented as 8-bit unsigned integer according the [ASCII](https://en.wikipedia.org/wiki/ASCII) standard. The following shows the actual values (in hexadecimal) to represent the given string.

```
T  h  e     q  u  i  c  k     b  r  o  w  n     f  o  x     j  u  m  p  s
54 68 65 20 71 75 69 63 6b 20 62 72 6f 77 6e 20 66 6f 78 20 6a 75 6d 70 73 20

o  v  e  r     t  h  e     l  a  z  y     d  o  g  .
6f 76 65 72 20 74 68 65 20 6c 61 7a 79 20 64 6f 67 2e
```

First, we divide the input string into 4-bit symbols.

```
T         h         e                   
0101 0100 0110 1000 0110 0101 0010 0000 
q         u         i         c         k
0111 0001 0111 0101 0110 1001 0110 0011 0110 1011 0010 0000
b         r         o         w         n
0110 0010 0111 0010 0110 1111 0111 0111 0110 1110 0010 0000
f         o         x
0110 0110 0110 1111 0111 1000 0010 0000
j         u         m         p         s
0110 1010 0111 0101 0110 1101 0111 0000 0111 0011 0010 0000
o         v         e         r
0110 1111 0111 0110 0110 0101 0111 0010 0010 0000
t         h         e
0111 0100 0110 1000 0110 0101 0010 0000
l         a         z         y
0110 1100 0110 0001 0111 1010 0111 1001 0010 0000
d         o         g         .
0110 0100 0110 1111 0110 0111 0010 1110
```

Now, we count the number of occurrences for each symbol and sort them according to the number of occurrences. As mentioned before, the most frequent 8 symbols are sorted according to the number of occurrences. For the rest of them, we arrange them according to the symbol value. Note that the symbols `0100` and `1000` have the same count, but we give the higher precedence to `0100` as it has a smaller value when interpreted as an unsigned integer.

```
Rank Symbol  Count  Code
0    0110    23     000 
1    0111    15     001
2    0010    12     010
3    0000    9      011
4    0101    6      1000
5    1111    4      1001
6    0100    3      1010
7    1000    3      1011
8    0001    2      11000
9    0011    2      11001
10   1001    2      11010
11   1010    2      11011
12   1011    1      11100
13   1100    1      11101
14   1101    1      11110
15   1110    2      11111
```

Once we get the above table, we can compress the original input data as follows.

```
T         h         e                   
1000 1010 000  1011 000 1000  010  011 
q         u         i         c         k
001 11000 001  1000 000 11010 000 11001 000 11100 010  011
b         r         o         w         n
000  010  001  010  000  1001 001  001  000 11111 010  011
f         o         x
000  000  000  1001 001  1011 010  011
j         u         m         p         s
000 11011 001  1000 000 11110 001  011  001 11001 010  011
o         v         e         r
000  1001 001  000  000  1000 001  010  010  011
t         h         e
001  1010 000  1011 000  1000 010  011
l         a         z         y
000 11101 000 11000 001 11011 001 11010 010  011
d         o         g         .
000  1010 000  1001 000  001  010  11111
```

We need to prefix the header information (__Rank table__ and __End info__) at the front of this bitstream. So, the final output will be as follows.

```
0110 0111 0010 0000 0101 1111 0100 1000    // Rank table
0010                                       // End info
1000 1010 0001 0110 0010 0001 0011 0011    
1000 0011 0000 0011 0100 0011 0010 0011    
1000 1001 1000 0100 0101 0000 1001 0010    
0100 0111 1101 0011 0000 0000 0100 1001
1011 0100 1100 0110 1100 1100 0000 1111
0001 0110 0111 0010 1001 1000 1001 0010
0000 0100 0001 0100 1001 1001 1010 0001
0110 0010 0001 0011 0001 1101 0001 1000
0011 1011 0011 1010 0100 1100 0101 0000
1001 0000 0101 0111 11..                   // .. denotes the padded bits (should be 0)
```

We can see that the original 44-byte input string has been compressed into 43 bytes. Not that much, huh?


## Problem specification

Write a C function named `encode()` that encodes the input binary data using a simplified Huffman coding. The prototype of `encode()` is as follows:

```
int encode(const char *inp, int inbytes, char *outp, int outbytes);
```

The first argument `inp` points to the memory address of the input data. The length of the input data (in bytes) is specified in the second argument `inbytes`. The encoded result should be stored starting from the address pointed to by ``outp``. Finally, the `outbytes` argument indicates the number of bytes allocated for the result by the caller.

The function `encode()` returns the actual length of the output in bytes including the header information and the encoded data (plus padded bits). If the size of the output exceeds the `outbytes`, it should return -1 and the contents of the output is ignored. When the `inbytes` is zero, `encode()` returns zero.




## Skeleton code

We provide you with the skeleton code for this project. It can be downloaded from Github at https://github.com/snu-csl/ca-pa1/. If you don't have the `git` utility, you need to install it first. You can install the `git` utility on Ubuntu by running the following command:
```
$ sudo apt install git
```
For MacOS, install the Xcode command line tools which come with `git`.

To download and build the skeleton code, please follow these steps:

```
$ git clone https://github.com/snu-csl/ca-pa1.git
$ cd ca-pa1
$ make
gcc -g -O2 -Wall   -c -o pa1.o pa1.c
gcc -g -O2 -Wall   -c -o pa1-main.o pa1-main.c
gcc -g -O2 -Wall   -c -o pa1-test.o pa1-test.c
gcc -o pa1 pa1.o pa1-main.o pa1-test.o
```

The result of a sample run looks like this:

```
$ ./pa1
-------- Test #0
[ Data ] (char) {0xca, 0x20, 0x20}
[Result] length == 0
[Answer] length == 7
[Result] encoded - (empty)
[Answer] encoded - 02 ac 13 45 26 88 20 
-------- WRONG!

-------- Test #1
[ Data ] (char) {0xde, 0xad, 0xbe, 0xef}
[Result] length == 0
[Answer] length == 8
[Result] encoded - (empty)
[Answer] encoded - ed ab f0 12 32 11 60 40 
-------- WRONG!

(... more test cases below ...)
```

## Restrictions

* You are not allowed to use any library functions (including `printf()`) inside the `pa1.c` file. 

* Do not include any header file in the `pa1.c` file.

* Your solution should finish within a reasonable time. If your code does not finish within a predefined threshold (e.g., 5 sec), it will be killed.


## Hand in instructions

* In order to submit your solution, you need to register an account to the submission server at https://sys.snu.ac.kr
  * You must enter your real name & student ID
  * Wait for an approval from the TA
* Note that the submission server is only accessible inside the SNU campus network. If you want off-campus access to the submission server, please send your IP to the TAs via email (`snucsl.ta` AT `gmail`)
* Upload only the `pa1.c` file to the submission server

## Logistics

* You will work on this project alone.
* Only the upload submitted before the deadline will receive the full credit. 25% of the credit will be deducted for every single day delay.
* __You can use up to 4 _slip days_ during this semester__. If your submission is delayed by 1 day and if you decided to use 1 slip day, there will be no penalty. In this case, you should explicitly declare the number of slip days you want to use in the QnA board of the submission server after each submission. Saving the slip days for later projects is highly recommended!
* Any attempt to copy others' work will result in heavy penalty (for both the copier and the originator). Don't take a risk.

Have fun!

[Jin-Soo Kim](mailto:jinsoo.kim_AT_snu.ac.kr)  
[Systems Software and Architecture Laboratory](http://csl.snu.ac.kr)  
[Dept. of Computer Science and Engineering](http://cse.snu.ac.kr)  
[Seoul National University](http://www.snu.ac.kr)
