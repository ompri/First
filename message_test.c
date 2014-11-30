#include "aux_macros.h"
#include <stdlib.h>
#include <stdbool.h>
#include "message.h"

bool message_test_Combo();

int main() {
	RUN_TEST(message_test_Combo)

	return 0;
}

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

bool message_test_Combo() {
	bool final = true;
	unsigned char imgData[] = {15,240};
	const unsigned char* imgDataPtr = imgData;
	Message img = messageImageCreate(1,&imgDataPtr, 2, 1);
	Message txt = messageTextCreate(2,"ab\nc\n");
	Message txt2 = messageTextCreate(3,"d");
	Message txt3 = messageTextCreate(3,"3");
	bool isPalindrome = true;
	TEST_EQUALS(final, messageIsPalindrome(txt, &isPalindrome), MESSAGE_SUCCESS)
	TEST_EQUALS(final, isPalindrome, false)
	TEST_EQUALS(final, messageNumberOfLines(txt), 3)
	messageConcat(txt2, txt3);
	TEST_EQUALS(final, messageLength(txt2), 3)
	char character;
	messageGetCharacter(txt, 1, 0, &character);
	TEST_EQUALS(final, character, 'c')
	messageInvertColors(img);
	unsigned char color;
	messageGetColor(img, 0, 1, &color);
	TEST_EQUALS(final, color, 15)
	messageRotateClockwise(img);
	messageRotateClockwise(img);
	messageGetColor(img, 0, 0, &color);
	TEST_EQUALS(final, color, 15)
	messageRotateAntiClockwise(img);
	messageGetColor(img, 1, 0, &color);
	TEST_EQUALS(final, color, 15)
	bool identical;
	messageCompare(img, txt, &identical);
	TEST_EQUALS(final, identical, false)
	Message img2 = messageCopy(img);
	messageCompare(img, img2, &identical);
	TEST_EQUALS(final, identical, true)

	int senderID;
	messageGetSenderID(img, &senderID);
	TEST_EQUALS(final, senderID, 1)
	MessageType type;
	messageGetType(txt, &type);
	TEST_EQUALS(final, type, MESSAGE_TEXT)

	messageDestroy(img);
	messageDestroy(img2);
	messageDestroy(txt);
	messageDestroy(txt2);
	messageDestroy(txt3);

	return final;
}
