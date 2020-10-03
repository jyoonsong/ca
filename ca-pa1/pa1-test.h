//---------------------------------------------------------------
//
//  4190.308 Computer Architecture (Fall 2020)
//
//  Project #1: Compressing Data with Huffman Coding
//
//  September 9, 2020
//
//  Injae Kang (abcinje@snu.ac.kr)
//  Sunmin Jeong (sunnyday0208@snu.ac.kr)
//  Systems Software & Architecture Laboratory
//  Dept. of Computer Science and Engineering
//  Seoul National University
//
//---------------------------------------------------------------

#ifndef _PA1_TEST_H_
#define _PA1_TEST_H_

typedef enum {
	CHAR, STRING, INT, FLOAT,
} datatype;

typedef struct {
	void *input;
	int input_len;
	int input_type_len;
	char *ans;
	int ans_len;
	datatype dtype;
} testcase;

extern int num_testcases;
extern testcase tc[];
void init_testcases(void);

#endif /* _PA1_TEST_H_ */
