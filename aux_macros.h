/*
 * aux_macros.h
 *
 *  Created on: 7 баеч 2014
 *      Author: user
 */

#ifndef AUX_MACROS_H_
#define AUX_MACROS_H_

#include <stdio.h>

#define INTEGER_ERROR_VALUE -1

#define CHECK_NULL_AND_RETURN_NULL(pointer) if (pointer == NULL) { \
												return NULL; \
											}

#define TEST_EQUALS(res_arg, a,b) if (res_arg && (a != b)) { \
								res_arg = false; \
							}

#define RUN_TEST(name)  printf("Running "); \
						printf(#name);		\
						printf("... ");		\
						if (!name()) { \
							printf("[FAILED]\n");		\
							return INTEGER_ERROR_VALUE; \
						}								\
						printf("[SUCCESS]\n");

#endif /* AUX_MACROS_H_ */
