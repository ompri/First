#include "aux_macros.h"
#include <assert.h>
#include <stdlib.h>
#include "conversation.h"
#include <stdbool.h>

bool conversation_test_Combo();

// create and a new allocated copy of an image
// of the form [1, 2, 3
//                              4, 5, 6
//                              7, 8, 9]
static unsigned char** allocateNewImage(unsigned int height, unsigned int width,
                bool isZero, int lineDelta) {
        unsigned char** image = malloc(height * sizeof (char*));
        for (int i=0; i < height; i++) {
                image[i] = malloc(width * sizeof (char));
        }
        for (int i=0; i < height; i++) {
                for (int j=0; j < width; j++) {
                        if (isZero) {
                                image[i][j] = 0;
                        }
                        else {
                                image[i][j] = i*(width + lineDelta) + (j+1);
                        }
                }
        }
        return image;
}

static void deleteImage(unsigned char** image,
                unsigned int height, unsigned int width) {
        for (int i=0; i < height; i++) {
                free(image[i]);
        }
        free(image);
}

Message generateImageMessage(unsigned int height, unsigned int width,
                bool isZero, int senderID, int lineDelta) {
        unsigned char** imageToPass = allocateNewImage(height,width, isZero, lineDelta);
        const unsigned char* const* constImgToPass =
                        (const unsigned char* const*) imageToPass;
        Message msg = messageImageCreate(senderID, constImgToPass, width, height);
        deleteImage(imageToPass,height,width);
        return msg;
}


int main() {
	RUN_TEST(conversation_test_Combo)

	return 0;
}

bool conversation_test_Combo() {
	bool final = true;

	Conversation conv1 = conversationCreate(3);
	Conversation conv2 = ConversationCopy(conv1);

	Message txt101 = messageTextCreate(101,"rrr");
	Message txt102 = messageTextCreate(102,"rrr");
	Message img101 = generateImageMessage(1,2,false,101,0);
	Message img102 = generateImageMessage(1,2,false,102,0);
	TEST_EQUALS(final, conversationAddMessage(conv1, txt101), CONVERSATION_SUCCESS)
	TEST_EQUALS(final, conversationAddMessage(conv1, txt101), CONVERSATION_SUCCESS)
	TEST_EQUALS(final, conversationNumberOfUsers(conv2), 0)
	TEST_EQUALS(final, conversationAddMessage(conv2, img102), CONVERSATION_SUCCESS)
	int mostMessagesId;
	TEST_EQUALS(final, conversationMostImages(conv2, &mostMessagesId), CONVERSATION_SUCCESS)
	TEST_EQUALS(final, mostMessagesId, 102)
	TEST_EQUALS(final, conversationMostTextMessages(conv1, &mostMessagesId), CONVERSATION_SUCCESS)
	TEST_EQUALS(final, mostMessagesId, 101)
	Message outMessage;
	TEST_EQUALS(final, conversationLastMessage(conv2, 102, &outMessage), CONVERSATION_SUCCESS)
	bool identical;
	TEST_EQUALS(final, messageCompare(outMessage, img102, &identical), MESSAGE_SUCCESS)
	TEST_EQUALS(final, identical, true)
	TEST_EQUALS(final, conversationDeleteMessage(conv2, 102, 0), CONVERSATION_SUCCESS)
	TEST_EQUALS(final, conversationLength(conv1), 2)


	messageDestroy(img101);
	messageDestroy(img102);
	messageDestroy(outMessage);
	messageDestroy(txt101);
	messageDestroy(txt102);

	conversationDestroy(conv1);
	conversationDestroy(conv2);

	return final;
}
