// Fill out your copyright notice in the Description page of Project Settings.


#include "RpcConnectionManager.h"


RpcConnectionManager::RpcConnectionManager()
{
}

RpcConnectionManager::~RpcConnectionManager()
{
}

int RpcConnectionManager::SendTo(const char* buffer, int len, int flags, int connection_id)
{
	TArray<int32> scheduledMessage((int32*)buffer,len/4);
	sendSchedule.AddTail(scheduledMessage);
	return 0;
}

int RpcConnectionManager::RecvFrom(char* buffer, int len, int flags, int* connection_id)
{
	if (receiveSchedule.Num() == 0)
		return -1;
	auto msg = receiveSchedule.GetHead();
	
	auto msgVal = msg->GetValue();
	auto rec = (char*)msgVal.GetData();
	auto leng = msgVal.Num()*4; // int* to char* size
	if (leng == 0)
		return -1;
	memcpy(buffer, rec, leng);
	delete[] rec;
	delete msg;
	receiveSchedule.RemoveNode(msg);
	*connection_id = playerIndex;
	return leng;
}
