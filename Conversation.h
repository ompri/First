#ifndef CONVERSATION_H_
#define CONVERSATION_H_

#include <stdlib.h>
#include <stdbool.h>
#include "message.h"

typedef struct conversation_t* Conversation;
struct conversation_t {
	unsigned int maxNumOfMessages;
	int currIdx;
	Message* messages;
};

typedef enum {
	CONVERSATION_OUT_OF_MEMORY,
	CONVERSATION_SUCCESS,
	CONVERSATION_NULL_ARGUMENT,
	CONVERSATION_BAD_ID,
	CONVERSATION_FULL,
	CONVERSATION_NO_MESSAGES,
	CCONVERSATION_BAD_ID,
	CONVERSATION_OUT_OF_BOUNDS
}ConversationResult;


#endif /* CONVERSATION_H_ */
