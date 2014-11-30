
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "message.h"

// an edit
//TODO: should we malloc the data attribute at message->content.image.data
// new edit from eclips
Message messageImageCreate(int senderID, const unsigned char * const * image,
		unsigned width, unsigned height) {

	if (image == NULL || width == 0 || height == 0)
		return NULL;

	Message message = malloc(sizeof(*message));
	if (!message) {
		return NULL;

	message->content.image.data = malloc(height * sizeof (char*));
	if (!message->content.image.data){
		return NULL;
	}
	for (int i=0; i < height; i++) {
		message->content.image.data[i] = malloc(width * sizeof (char));
		        }

	}

	message->senderID = senderID;
	message->type = MESSAGE_IMAGE;
	message->content.image.width = width;
	message->content.image.height = height;


	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			message->content.image.data[i][j] = image[i][j];
		}
	}


	return message;
}

/* new demo function
MessageResult messageInvertColors(Message message) {
	if (!message) {
		return MESSAGE_NULL_ARGUMENT;
	}
*/

MessageResult messageInvertColors(Message message) {
	if (!message) {
		return MESSAGE_NULL_ARGUMENT;
	}

	if (message->type == MESSAGE_TEXT) {
		return MESSAGE_WRONG_TYPE;
	}

	for (int i = 0; i < message->content.image.height; ++i) {
		for (int j = 0; j < message->content.image.width; ++j) {
			message->content.image.data[i][j] =
					(255 - message->content.image.data[i][j]);
		}
	}
	return MESSAGE_SUCCESS;

}

/**
 TODO: maybe switch between height and width
 TODO: something with ptrs?
 */


static MessageResult messageToClock(Message msg) {

	unsigned char ** tmp= malloc(sizeof(unsigned char)*msg->content.image.height*msg->content.image.width);
	if(!tmp)
		return MESSAGE_OUT_OF_MEMORY;

	for (int i = 0; i < msg->content.image.height; i++) {
		for (int j = 0; j < msg->content.image.width; j++) {
			//tmp[j][msg->content.image.width - i]=
			tmp[j][msg->content.image.width - i]=msg->content.image.data[i][j];

		}
	}

	int lastHeight = msg->content.image.height;
	msg->content.image.height = msg->content.image.width;
	msg->content.image.height = lastHeight;

	for (int i = 0; i < msg->content.image.height; ++i) {
			for (int j = 0; j < msg->content.image.width; ++j) {
				msg->content.image.data[i][j] = tmp[i][j];
			}

		}

	free(tmp);
	return MESSAGE_SUCCESS;

}

MessageResult messageRotateClockwise(Message message) {
	if (!message) {
		return MESSAGE_NULL_ARGUMENT;
	}

	if (message->type == MESSAGE_TEXT) {
		return MESSAGE_WRONG_TYPE;
	}

	return messageToClock(message);
}

MessageResult messageRotateAntiClockwise(Message message) {
	if (!message) {
		return MESSAGE_NULL_ARGUMENT;
	}

	if (message->type == MESSAGE_TEXT) {
		return MESSAGE_WRONG_TYPE;
	}

	MessageResult mr;
	mr=messageToClock(message);
	if(mr!=MESSAGE_SUCCESS)
		return mr;
	mr=messageToClock(message);
	if(mr!=MESSAGE_SUCCESS)
		return mr;
	mr=messageToClock(message);
	if(mr!=MESSAGE_SUCCESS)
		return mr;

	return MESSAGE_SUCCESS;
}

MessageResult messageGetColor(const Message message, unsigned row,
		unsigned column, unsigned char* outColor) {

	if (!message) {
		return MESSAGE_NULL_ARGUMENT;
	}

	if (message->type == MESSAGE_TEXT) {
		return MESSAGE_WRONG_TYPE;
	}

	if (message->content.image.height <= row
			|| message->content.image.width <= column) {
		return MESSAGE_OUT_OF_BOUNDS;
	}
	*outColor = message->content.image.data[row][column];
	return MESSAGE_SUCCESS;
}

//TODO: should we malloc the text attribute at message->content.text
Message messageTextCreate(int senderID, const char* text){

	if(!text){
		return NULL;
	}

	Message msg= malloc(sizeof(*msg));

	if(!msg){
		return NULL;
	}

	strcpy(msg->content.text,text);
	msg->senderID=senderID;
	msg->type=MESSAGE_TEXT;


	return msg;
}

static bool isPalindrome(const char* line){
	int len=strlen(line);
	if(len==0) return true;
	if(len%2==0){
		for(int i=0;i<len/2;i++)
			if(line[i]!=line[len-i-1])
				return false;

		return true;
	}else{
		for(int i=0;i<(len-1)/2;i++)
			if(line[i]!=line[len-i-1])
				return false;

		return true;
	}

}

/**
we return a char:
o-out of memroy
t-true
f-false
*/

static char isTextPalindrome(const char* text){

	char* tempstr = malloc((strlen(text)+1)*sizeof(char));
	if(!tempstr)
		return 'o';
	strcpy(tempstr, text);

	char* token = strtok(tempstr, "\n");
	while (token) {
		if(!isPalindrome(token)){
			free(tempstr);
			return 'f';
		}

	    token = strtok(NULL, "\n");
	}

	free(tempstr);
	return 't';

}

MessageResult messageIsPalindrome(const Message message,
		bool* outIsPalindrome){

	// TODO: should EDIT and considr LF?

	if(!message)
		return MESSAGE_NULL_ARGUMENT;

	if(message->type !=MESSAGE_TEXT)
		return MESSAGE_WRONG_TYPE;

	char code=isTextPalindrome(message->content.text);
	if(code=='o'){
		return MESSAGE_OUT_OF_MEMORY;
	}else if(code=='t'){
		*outIsPalindrome=true;
	}else if(code=='f'){
		*outIsPalindrome=false;
	}

	return MESSAGE_SUCCESS;

}

int messageNumberOfLines(const Message message){

	if(!message)
		return -1;

	if(message->type !=MESSAGE_TEXT)
		return -1;

	int count=1;
	int len=strlen(message->content.text);
	for(int i=0;i<len;i++)
		if(message->content.text[i]=='\n')
			count++;

	return count;


}

MessageResult messageConcat(Message message1, const Message message2){

	if(!message1||!message2)
		return MESSAGE_NULL_ARGUMENT;

	if(message1->type !=MESSAGE_TEXT||message2->type !=MESSAGE_TEXT)
		return MESSAGE_WRONG_TYPE;

	if(message1->senderID!=message2->senderID)
		return MESSAGE_DIFFERENT_SENDER;

	strcat(message1->content.text,"\n");
	strcat(message1->content.text,message2->content.text);

	return MESSAGE_SUCCESS;

}

int messageLength(const Message message){

	if(!message)
		return -1;

	if(message->type !=MESSAGE_TEXT)
		return -1;

	return strlen(message->content.text);

}

static MessageResult getCharacter(const char* text, int line, int index, char* outValue){

	char* tempstr = malloc((strlen(text)+1)*sizeof(char));
	if(!tempstr)
		return MESSAGE_OUT_OF_MEMORY;
	strcpy(tempstr, text);

	char* token = strtok(tempstr, "\n");//0-line
	int count=0;

	while (token) {
		if(line == count)
			break;

	    token = strtok(NULL, "\n");
	}

	if( index > strlen(token) -1)
		return MESSAGE_OUT_OF_BOUNDS;

	*outValue=token[index];

	free(tempstr);
	return MESSAGE_SUCCESS;

}

MessageResult messageGetCharacter(const Message message, int line,
	int index, char* outValue){

	if(!message)
		return MESSAGE_NULL_ARGUMENT;

	if(message->type !=MESSAGE_TEXT)
		return MESSAGE_WRONG_TYPE;

	if( line > strlen(message->content.text) -1)
		return MESSAGE_OUT_OF_BOUNDS;

	return getCharacter(message->content.text,line,index,outValue);

}


/**
 * both pictures are assumed to be of the same sizes
 * TODO: maybe width and height should be switched
 */
static bool charMatrixCmp(unsigned char ** mat1,unsigned char ** mat2,
		unsigned int width,unsigned int height){

	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			if(mat1[i][j]!=mat2[i][j])
				return false;
		}
	}
	return true;

}

MessageResult messageCompare(const Message message1, const Message message2,
		bool* outIdentical) {

	if (!message1 || !message2){
		return MESSAGE_NULL_ARGUMENT;
	}

	bool b=true;

	if(message1->senderID != message2->senderID)
		b=false;

	if(message1->type != message2->type)
		b=false;

	if(message1->type==MESSAGE_TEXT)
		if(strcmp(message1->content.text,message2->content.text))
			b=false;

	if(message1->type==MESSAGE_IMAGE){
			if(message1->content.image.width != message2->content.image.width)
				b=false;

			if(message1->content.image.height != message2->content.image.height)
				b=false;

			if(charMatrixCmp(
					message1->content.image.data,
					message2->content.image.data,
					message1->content.image.width,
					message1->content.image.height) == false)
				b=false;
	}

	*outIdentical=b;

	return MESSAGE_SUCCESS;

}

MessageResult messageGetType(const Message message, MessageType* outType) {
if (!message) {
	return MESSAGE_NULL_ARGUMENT;
}

*outType = message->type;
return MESSAGE_SUCCESS;
}

MessageResult messageGetSenderID(const Message message, int* outSenderID) {
	if (!message) {
		return MESSAGE_NULL_ARGUMENT;
	}

	*outSenderID = message->senderID;
	return MESSAGE_SUCCESS;
	}

void messageDestroy(Message message) {
	free(message);
}



Message messageCopy(const Message message) {

	if (!message) {
		return NULL;
	}

	MessageType type =message->type;

	Message msg=NULL;
	if (type == MESSAGE_TEXT) {

		char* copy = malloc((strlen(message->content.text)+1)*sizeof(char));
		if(!copy)
			return NULL;
		strcpy(copy,message->content.text);

		msg=messageTextCreate(message->senderID, copy);
	}else{

		/*
		 TODO:
		 	 1)maybe we should use messageImageCreate (there's a const problem)
		 	 2)if we initate it here, should we use malloc for data?\
		 	  in different words, we should copy the code from messageImageCreate
		 */

		msg = malloc(sizeof(*message));
		if (!msg) {
			return NULL;
		}

		msg->senderID = message->senderID;
		msg->type = MESSAGE_IMAGE;
		msg->content.image.width = message->content.image.width;
		msg->content.image.height = message->content.image.height;


		for (int i = 0; i < msg->content.image.height; i++) {
			for (int j = 0; j < msg->content.image.width; j++) {
				msg->content.image.data[i][j] = message->content.image.data[i][j];
			}
		}


	}

	return msg;

}

