/*
 * test1.c
 *
 *  Created on: 27 бреб 2014
 *      Author: amd
 */

#include "aux_macros.h"
#include <assert.h>
#include <stdlib.h>
#include "conversation.h"
#include <stdbool.h>

bool conversation_test_Combo();
bool conversation_test_Extra();

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
	RUN_TEST(conversation_test_Extra)

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
	Message outMessage = NULL;
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

bool conversation_test_Extra() {
	bool final = true;

	TEST_EQUALS(final, conversationCreate(0), NULL)

	Conversation convSmall = conversationCreate(3);
	Conversation convLarge = conversationCreate(2000);

	Message txt101 = messageTextCreate(101, "one");
	Message txt102 = messageTextCreate(102, "two");
	Message img101 = generateImageMessage(10, 20, false, 101, 0);
	Message img102 = generateImageMessage(10, 20, false, 102, 0);

	ConversationResult result;

	//////////////////////////////////////////////////////////////////////////
	// conversationAddMessage

	result = conversationAddMessage(convSmall, txt101);
	TEST_EQUALS(final, result, CONVERSATION_SUCCESS)
	result = conversationAddMessage(convSmall, txt102);
	TEST_EQUALS(final, result, CONVERSATION_SUCCESS)
	result = conversationAddMessage(convSmall, img101);
	TEST_EQUALS(final, result, CONVERSATION_SUCCESS)
	result = conversationAddMessage(convSmall, img102);
	TEST_EQUALS(final, result, CONVERSATION_FULL)

	for(int i = 0; i < 1000; i++) {
		Message tempMessage = messageTextCreate(999+i, "test");
		result = conversationAddMessage(convLarge, tempMessage);
		TEST_EQUALS(final, result, CONVERSATION_SUCCESS)
		messageDestroy(tempMessage);
	}
	for(int i = 0; i < 1000; i++) {
		Message tempMessage = generateImageMessage(10, 20, false, i, 0);
		result = conversationAddMessage(convLarge, tempMessage);
		TEST_EQUALS(final, result, CONVERSATION_SUCCESS)
		messageDestroy(tempMessage);
	}
	result = conversationAddMessage(convLarge, img102);
	TEST_EQUALS(final, result, CONVERSATION_FULL)

	//////////////////////////////////////////////////////////////////////////
	// conversationNumberOfUsers

	TEST_EQUALS(final, conversationNumberOfUsers(NULL), -1)
	TEST_EQUALS(final, conversationNumberOfUsers(convSmall), 2)
	TEST_EQUALS(final, conversationNumberOfUsers(convLarge), 1999)

	//////////////////////////////////////////////////////////////////////////
	// conversationMostTextMessages

	int outUserID;
	TEST_EQUALS(final, conversationMostTextMessages(NULL, &outUserID), CONVERSATION_NULL_ARGUMENT)

	{
		Conversation convTemp = conversationCreate(10);

		int outUserID;
		TEST_EQUALS(final, conversationMostTextMessages(convTemp, &outUserID), CONVERSATION_NO_MESSAGES)

		result = conversationAddMessage(convTemp, img102);
		TEST_EQUALS(final, conversationMostTextMessages(convTemp, &outUserID), CONVERSATION_NO_MESSAGES)

		Message tempMessage1 = messageTextCreate(1, "test");
		Message tempMessage2 = messageTextCreate(2, "test");

		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage2);
		result = conversationAddMessage(convTemp, tempMessage2);
		result = conversationAddMessage(convTemp, tempMessage2);
		result = conversationAddMessage(convTemp, tempMessage1);

		TEST_EQUALS(final, conversationMostTextMessages(convTemp, &outUserID), CONVERSATION_SUCCESS)
		TEST_EQUALS(final, outUserID, 1)

		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage2);

		TEST_EQUALS(final, conversationMostTextMessages(convTemp, &outUserID), CONVERSATION_SUCCESS)
		TEST_EQUALS(final, outUserID, 2)

		result = conversationAddMessage(convTemp, tempMessage1);

		TEST_EQUALS(final, conversationMostTextMessages(convTemp, &outUserID), CONVERSATION_SUCCESS)
		TEST_EQUALS(final, outUserID, 1)

		messageDestroy(tempMessage1);
		messageDestroy(tempMessage2);

		conversationDestroy(convTemp);
	}

	//////////////////////////////////////////////////////////////////////////
	// conversationMostImages

	TEST_EQUALS(final, conversationMostImages(NULL, &outUserID), CONVERSATION_NULL_ARGUMENT)

	{
		Conversation convTemp = conversationCreate(10);

		int outUserID;
		TEST_EQUALS(final, conversationMostImages(convTemp, &outUserID), CONVERSATION_NO_MESSAGES)

		result = conversationAddMessage(convTemp, txt101);
		TEST_EQUALS(final, conversationMostImages(convTemp, &outUserID), CONVERSATION_NO_MESSAGES)

		Message tempMessage1 = generateImageMessage(10, 20, false, 1, 0);
		Message tempMessage2 = generateImageMessage(10, 20, false, 2, 0);

		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage2);
		result = conversationAddMessage(convTemp, tempMessage2);
		result = conversationAddMessage(convTemp, tempMessage2);
		result = conversationAddMessage(convTemp, tempMessage1);

		TEST_EQUALS(final, conversationMostImages(convTemp, &outUserID), CONVERSATION_SUCCESS)
		TEST_EQUALS(final, outUserID, 1)

		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage2);

		TEST_EQUALS(final, conversationMostImages(convTemp, &outUserID), CONVERSATION_SUCCESS)
		TEST_EQUALS(final, outUserID, 2)

		result = conversationAddMessage(convTemp, tempMessage1);

		TEST_EQUALS(final, conversationMostImages(convTemp, &outUserID), CONVERSATION_SUCCESS)
		TEST_EQUALS(final, outUserID, 1)

		messageDestroy(tempMessage1);
		messageDestroy(tempMessage2);

		conversationDestroy(convTemp);
	}

	//////////////////////////////////////////////////////////////////////////
	// conversationLastMessage

	Message msgResult;
	TEST_EQUALS(final, conversationLastMessage(NULL, 2000, &msgResult), CONVERSATION_NULL_ARGUMENT)
	TEST_EQUALS(final, conversationLastMessage(convLarge, 1999, &msgResult), CONVERSATION_BAD_ID)
	TEST_EQUALS(final, conversationLastMessage(convLarge, -1, &msgResult), CONVERSATION_BAD_ID)

	{
		Conversation convTemp = conversationCreate(10);

		result = conversationAddMessage(convTemp, img102);

		Message tempMessage1 = messageTextCreate(1, "user1");
		Message tempMessage21 = messageTextCreate(2, "user2msg1");
		Message tempMessage22 = messageTextCreate(2, "user2msg2");
		Message tempMessageOut;

		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage21);
		result = conversationAddMessage(convTemp, tempMessage22);
		result = conversationAddMessage(convTemp, tempMessage21);
		result = conversationAddMessage(convTemp, tempMessage1);
		result = conversationAddMessage(convTemp, tempMessage22);
		result = conversationAddMessage(convTemp, tempMessage1);

		{
			Conversation convTemp2 = ConversationCopy(convTemp);

			result = conversationLastMessage(convTemp2, 2, &tempMessageOut);
			TEST_EQUALS(final, result, CONVERSATION_SUCCESS)

			bool isIdentical = false;
			messageCompare(tempMessageOut, tempMessage22, &isIdentical);
			TEST_EQUALS(final, isIdentical, true)

			messageDestroy(tempMessageOut);

			conversationDeleteMessage(convTemp2, 2, 3);

			result = conversationLastMessage(convTemp2, 2, &tempMessageOut);
			TEST_EQUALS(final, result, CONVERSATION_SUCCESS)

			isIdentical = false;
			messageCompare(tempMessageOut, tempMessage21, &isIdentical);
			TEST_EQUALS(final, isIdentical, true)

			messageDestroy(tempMessageOut);

			conversationDestroy(convTemp2);
		}

		result = conversationLastMessage(convTemp, 2, &tempMessageOut);
		TEST_EQUALS(final, result, CONVERSATION_SUCCESS)

		conversationDestroy(convTemp);

		bool isIdentical = false;
		messageCompare(tempMessageOut, tempMessage22, &isIdentical);
		TEST_EQUALS(final, isIdentical, true)

		messageDestroy(tempMessageOut);

		result = conversationLastMessage(convSmall, 101, &tempMessageOut);
		TEST_EQUALS(final, result, CONVERSATION_SUCCESS)

		isIdentical = false;
		messageCompare(tempMessageOut, img101, &isIdentical);
		TEST_EQUALS(final, isIdentical, true)

		messageDestroy(tempMessage1);
		messageDestroy(tempMessage21);
		messageDestroy(tempMessage22);
		messageDestroy(tempMessageOut);
	}

	//////////////////////////////////////////////////////////////////////////
	// conversationDeleteMessage
	// conversationLength
	// ConversationCopy

	TEST_EQUALS(final, conversationLength(NULL), -1)
	TEST_EQUALS(final, conversationLength(convSmall), 3)
	TEST_EQUALS(final, conversationLength(convLarge), 2000)

	{
		Conversation convTemp = ConversationCopy(convLarge);
		TEST_EQUALS(final, conversationLength(convTemp), 2000)
		conversationDestroy(convTemp);
	}

	TEST_EQUALS(final, conversationDeleteMessage(convLarge, 1999, 0), CONVERSATION_BAD_ID)
	TEST_EQUALS(final, conversationDeleteMessage(convLarge, 1998, 1), CONVERSATION_OUT_OF_BOUNDS)
	TEST_EQUALS(final, conversationDeleteMessage(convLarge, 1998, 0), CONVERSATION_SUCCESS)

	//////////////////////////////////////////////////////////////////////////

	messageDestroy(img101);
	messageDestroy(img102);
	messageDestroy(txt101);
	messageDestroy(txt102);

	conversationDestroy(convSmall);
	conversationDestroy(convLarge);

	return final;
}


