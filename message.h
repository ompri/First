
#ifndef MESSAGE_H_
#define MESSAGE_H_

typedef enum {
	MESSAGE_TEXT, MESSAGE_IMAGE
} MessageType;

typedef union {
	char * text;

	struct {
		unsigned char ** data;
		unsigned int width;
		unsigned int height;
	} image;
} Content;

/**
 * The message structure
 */
typedef struct message_t* Message;
struct message_t {
	int senderID;
	Content content;
	MessageType type;
};

typedef enum {
	MESSAGE_OUT_OF_MEMORY,
	MESSAGE_SUCCESS,
	MESSAGE_NULL_ARGUMENT,
	MESSAGE_WRONG_TYPE,
	MESSAGE_OUT_OF_BOUNDS,
	MESSAGE_DIFFERENT_SENDER
} MessageResult;

Message messageImageCreate(int senderID, const unsigned char * const * image,
		unsigned width, unsigned height);

MessageResult messageInvertColors(Message message);

MessageResult messageRotateClockwise(Message message);

MessageResult messageRotateAntiClockwise(Message message);

MessageResult messageGetColor(const Message message, unsigned row,
		unsigned column, unsigned char* outColor);

Message messageTextCreate(int senderID, const char* text);

MessageResult messageIsPalindrome(const Message message,
bool* outIsPalindrome);

int messageNumberOfLines(const Message message);

MessageResult messageConcat(Message message1, const Message message2);

int messageLength(const Message message);

/*MessageResult char getCharacter(const char* text, int line, int index,
		char*outValue);*/

MessageResult messageGetCharacter(const Message message, int line, int index,
	char* outValue);

MessageResult messageCompare(const Message message1, const Message message2,
bool* outIdentical);

MessageResult messageGetType(const Message message, MessageType* outType);

MessageResult messageGetSenderID(const Message message, int* outSenderID);

void messageDestroy(Message message);

Message messageCopy(const Message message);


#endif /* MESSAGE_H_ */








