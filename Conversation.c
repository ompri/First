#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "conversation.h"
#include "message.h"

/**

it is assumed that conversion is a struct:


typedef struct conversation_t* Conversation;
struct conversation_t {
	int maxNumOfMessages;
	int currIdx;
	Message* messages;
};

*/

Conversation conversationCreate( unsigned int maxNumOfMessages){
	if(maxNumOfMessages==0)
		return NULL;

	Conversation conv = malloc(sizeof(*conv));
	if (!conv) {
		return NULL;
	}

	conv->maxNumOfMessages=maxNumOfMessages;
	conv->currIdx=0;

	conv->messages=malloc(maxNumOfMessages*sizeof(Message));
	if (!(conv->messages)) {
		free(conv);
		return NULL;
	}

	return conv;

}

ConversationResult conversationAddMessage(Conversation conversation,
Message message){
	if(!conversation||!message)
		return CONVERSATION_NULL_ARGUMENT;

	if(conversation->currIdx==conversation->maxNumOfMessages-1)
		return CONVERSATION_FULL;

	conversation->messages[conversation->currIdx]=messageCopy(message);

	conversation->currIdx++;

	return CONVERSATION_SUCCESS;

}

int conversationNumberOfUsers(const Conversation conversation){
	if(!conversation)
		return -1;

	int count=0;
	int* ids=malloc(conversation->currIdx*sizeof(int));
	if(!ids)
		return CONVERSATION_OUT_OF_MEMORY;

	for(int i=0;i<conversation->currIdx;i++){
		int currID=conversation->messages[i]->senderID;
		if(i==0){
			for(int j=0;j<conversation->currIdx;j++)
				ids[j]=currID;
			count=1;
		}else{
			bool different=true;
			for(int j=0;j<conversation->currIdx;j++)
				if(ids[j] == currID)
					different=false;

			if(different){
				ids[count]=currID;
				count++;
			}

		}

	}

	free(ids);
	return count;

}

//TODO: is this allright
//TODO: check by F.A.Q
//TODO: check by F.A.Q other stuff

typedef struct senderInfo_t* SenderInfo;
struct senderInfo_t {
	int senderID;
	int textMsgCounter;
	int imageMsgCounter;
};

//TODO: make this function more readable
static SenderInfo* findSendersInfo(const Conversation conversation){

	SenderInfo* infoArray=malloc(conversation->currIdx*sizeof(SenderInfo));

	if(!infoArray)
		return NULL;

	int firstID;

	/*
	 the problem solved here that we can't know what SenderInfo->senderID is initiate with
	 so we initiate everyone of them with firstID value
	 the array at the end will be as follows:

	 {[firstID,],[secondID],[thirdID],,...[firstID],[firstID],[firstID]]}

	 at the end the values of firstID will be again, but for any realistic use it doesn't matter,
	 because we want to find the max and it is clear that last ones->counters will be initiate to 0
	 */

	int arraysLength=conversation->currIdx;

	for(int i=0;i<conversation->currIdx;i++){
		int currID=conversation->messages[i]->senderID;
		MessageType currType=conversation->messages[i]->type;

		if(i==0){//first we initaite everything to firstId

			//initate procces
			for(int j=0;j<arraysLength;j++){
				infoArray[j]->senderID=currID;
				infoArray[j]->textMsgCounter=0;
				infoArray[j]->imageMsgCounter=0;
			}

			//check type and counter++
			if(currType==MESSAGE_TEXT)
				infoArray[0]->textMsgCounter++;

			if(currType==MESSAGE_IMAGE)
				infoArray[0]->imageMsgCounter++;

			firstID=currID;

		}else{//it is not the first time

			if(currID==firstID){//if the message id is the same as first one's
				if(currType==MESSAGE_TEXT)
					infoArray[0]->textMsgCounter++;

				if(currType==MESSAGE_IMAGE)
					infoArray[0]->imageMsgCounter++;

			}else{//message's id is not firstID
				for(int j=1;j<arraysLength;j++){


					if(infoArray[j]->senderID==currID){//we found an array value with the same ID

						if(currType==MESSAGE_TEXT)
							infoArray[j]->textMsgCounter++;

						if(currType==MESSAGE_IMAGE)
							infoArray[j]->imageMsgCounter++;

					}

					if(infoArray[j]->senderID==firstID){//we have reached the end of valued array

						infoArray[j]->senderID=currID;

						if(currType==MESSAGE_TEXT)
							infoArray[j]->textMsgCounter++;

						if(currType==MESSAGE_IMAGE)
							infoArray[j]->imageMsgCounter++;

						break;

					}
				}


			}

		}
	}//for end

	return infoArray;
}

static void destroySendersInfo(SenderInfo* infoArray){
	free(infoArray);
}


ConversationResult conversationMostTextMessages(const Conversation conversation,
int* outUserID){

	if(!conversation||outUserID)
		return CONVERSATION_NULL_ARGUMENT;
	if(conversation->currIdx==0)
		return CONVERSATION_NO_MESSAGES;

	int maxID;
	int max;

	SenderInfo* infoArray=findSendersInfo(conversation);
	if(!infoArray)
		return CONVERSATION_OUT_OF_MEMORY;

	int firstID=infoArray[0]->senderID;
	for(int i=0;i<conversation->currIdx;i++){
		if(i!=0&&infoArray[i]->senderID==firstID)
			break;
		if(infoArray[i]->textMsgCounter>=max){
			max=infoArray[i]->textMsgCounter;
			maxID=infoArray[i]->senderID;
		}


	}

	destroySendersInfo(infoArray);

	if(!max)
		return CONVERSATION_NO_MESSAGES;

	*outUserID=maxID;

	return CONVERSATION_SUCCESS;

}

ConversationResult conversationMostImages(const Conversation conversation,
int* outUserID){

	if(!conversation||outUserID)
		return CONVERSATION_NULL_ARGUMENT;
	if(conversation->currIdx==0)
		return CONVERSATION_NO_MESSAGES;

	int maxID;
	int max;

	SenderInfo* infoArray=findSendersInfo(conversation);
	if(!infoArray)
		return CONVERSATION_OUT_OF_MEMORY;

	for(int i=0;i<conversation->currIdx;i++){

		if(infoArray[i]->imageMsgCounter>=max){
			max=infoArray[i]->imageMsgCounter;
			maxID=infoArray[i]->senderID;
		}


	}

	destroySendersInfo(infoArray);

	if(!max)
		return CONVERSATION_NO_MESSAGES;

	*outUserID=maxID;

	return CONVERSATION_SUCCESS;

}

//*********************************************************Amir's part starts here**************************************


ConversationResult conversationLastMessage(const Conversation conversation,
		int userID, Message* outMessage) {

	if (!conversation) {
		return CONVERSATION_NULL_ARGUMENT;
	}

	Message tmp = NULL;//here allocation is not needed beacuse this is a ptr that will get his value soon, 3 rows down
	for (int i = 0; i < conversation->currIdx; ++i) {
		if (conversation->messages[i]->senderID == userID) {
			tmp = conversation->messages[i];
		}
	}

	if (!tmp) {
		return CONVERSATION_BAD_ID;
	}

	*outMessage = messageCopy(tmp);
	return CONVERSATION_SUCCESS;
}


ConversationResult conversationDeleteMessage(Conversation conversation,
		int userID, int index) {
	if (!conversation) {
		return CONVERSATION_NULL_ARGUMENT;
	}
	if(!index)
		return CCONVERSATION_BAD_ID;
	//we should delete the message and              then move the array one point to the start

	int count=0;
	for (int i = 0; i < conversation->currIdx; +i++) {
		if(conversation->messages[i]->senderID == userID) {
			if (count == index){
				messageDestroy(conversation->messages[i]);
				for(int j=i+1;j<conversation->currIdx;j++){
					conversation->messages[j-1]=messageCopy(conversation->messages[j]);
					messageDestroy(conversation->messages[j]);
				}
				break;
			}else{
				count++;
			}
		}
	}

	if(count<index)
		return CONVERSATION_OUT_OF_BOUNDS;

	if(!count)
		return CONVERSATION_BAD_ID;

	return CONVERSATION_SUCCESS;
}

int conversationLength(const Conversation conversation) {
	if (!conversation) {
		return -1;
	}
	return conversation->currIdx;
}

Conversation ConversationCopy(const Conversation conversation) {
	if (!conversation) {
		return NULL;
	}

	Conversation tmp = conversationCreate(conversation->maxNumOfMessages);
	for (int i = 0; i < conversation->currIdx; ++i) {
		tmp->messages[i] = messageCopy(conversation->messages[i]);
		tmp->currIdx++;
	}
	return tmp;
}

void conversationDestroy(Conversation conversation) {
	if (conversation != NULL) {
		for (int i = 0; i < conversation->currIdx; ++i) {
			messageDestroy(conversation->messages[i]);
		}
		free(conversation);
	}

}
