/*
 * main.c
 *
 *  Created on: 16 ���� 2014
 *      Author: amd
 */

#include <string.h>
#include <stdlib.h>
bool invertAndDoubleString(char* str, bool doubleIsInvert, char * out1, char * out2) {
	if (doubleIsInvert) {
		out1 = malloc(strlen(str) * 2);
		out2 = malloc(strlen(str));
		if (!out1 || !out2)
			return false;
		for (int i = 0; i < strlen(str); i++)
		{
			out2[i] = str[i];
			out1[i] = str[strlen(str) - i];
			out1[i + strlen(str)] = str[strlen(str) - i];
		}
	}
	else {
		out1 = malloc(strlen(str) * 2);
		out2 = malloc(strlen(str));
		if (!out1 || !out2)
			return false;
		for (int i = 0; i < strlen(str); i++)
		{
			out2[i] = str[strlen(str) - i];
			out1[i] = str[i];
			out1[i + strlen(str)] = str[i];
		}
	}
	return true;
}

#include <stdio.h>

typedef struct node_t* Node;
struct node_t {
	int data;
	Node next;
};

Node xthToLast

