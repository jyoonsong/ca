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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED   "\033[0;31m"
#define GREEN "\033[0;32m"
#define CYAN  "\033[0;36m"
#define RESET "\033[0m"

#include "pa1-test.h"

int encode(const char *inp, int inbytes, char *outp, int outbytes);

void print_input(int num)
{
	switch (tc[num].dtype) {
	case CHAR:
		printf("(char) {");
		unsigned char *char_input = (unsigned char *)tc[num].input;
		for (int i = 0; i < tc[num].input_len; i++) {
			if (i > 0)
				printf(", ");
			printf("0x%02x", char_input[i]);
		}
		printf("}");
		break;
	case STRING:
		printf("(string) \"%s\"", (unsigned char *)tc[num].input);
		break;
	case INT:
		printf("(int) {");
		int *int_input = (int *)tc[num].input;
		for (int i = 0; i < tc[num].input_len / sizeof(int); i++) {
			if (i > 0)
				printf(", ");
			printf("%d", int_input[i]);
		}
		printf("}");
		break;
	case FLOAT:
		printf("(float) {");
		float *float_input = (float *)tc[num].input;
		for (int i = 0; i < tc[num].input_len / sizeof(float); i++) {
			if (i > 0)
				printf(", ");
			printf("%f", float_input[i]);
		}
		printf("}");
		break;
	default:
		fprintf(stderr, "print_data() failed: Unknown datatype\n");
		exit(1);
	}
}

void print_buffer(const char *buffer, int length)
{
	if (length == 0)
		printf("(empty)");
	else
		for (int i = 0; i < length; i++)
			printf("%02x ", (unsigned char)buffer[i]);
}

int test_routine(int num, bool buffer_is_enough)
{
	int ret = 0;
	int output_len = tc[num].ans_len;
	if (!buffer_is_enough)
		output_len /= 2;

	char *output = calloc(output_len, sizeof(char));

	/* Encode */
	int len = encode((const char *)tc[num].input, tc[num].input_len, output, output_len);

	/* Start checking */
	printf("%s-------- Test #%d%s\n", CYAN, num, RESET);

	printf("[ Data ] ");
	print_input(num);
	puts("");

	/* Check length */
	printf("[Result] length == %d\n", len);
	if (output_len < tc[num].ans_len) {
		printf("[Answer] length == -1 (output buffer is too short)\n");
		ret = (len == -1) ? 0 : 1;
		goto exit;
	}
	else
		printf("[Answer] length == %d\n", tc[num].ans_len);

	/* Check output */
	printf("[Result] encoded - ");
	print_buffer(output, len);
	puts("");

	printf("[Answer] encoded - ");
	print_buffer(tc[num].ans, tc[num].ans_len);
	puts("");

	if (len != tc[num].ans_len)
		ret = 1;
	else if (!output || memcmp(output, tc[num].ans, len))
		ret = 1;

	/* Finish checking */
exit:
	if (ret)
		printf("%s-------- %sWRONG!%s", CYAN, RED, RESET);
	else
		printf("%s-------- %sCORRECT!%s", CYAN, GREEN, RESET);

	puts("\n");
	free(output);
	return ret;
}

int main(void)
{
	init_testcases();

	int ret = 0;
	for (int i = 0; i < num_testcases; i++)
		ret |= test_routine(i, true);
	return ret;
}
