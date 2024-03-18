// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

typedef zmq::send_flags ComSendFlags;
typedef zmq::recv_flags ComRecvFlags;

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FSocketCommunicationServer
{
public:

	FSocketCommunicationServer(const std::string SendAddress, const std::string RecvAddress, const int Timeout);
	~FSocketCommunicationServer();

	bool EventOccured();

	bool SendArrayBuffer(const float* MessageData, size_t Size, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArrayBuffer(const float MessageData[], ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArrayBuffer(const TArray<float>& ArrayMessage, ComSendFlags SendFlags = ComSendFlags::none);

	bool SendArrayMessage(const float* MessageData, size_t Size, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArrayMessage(const float MessageData[], ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArrayMessage(const TArray<float>& ArrayMessage, ComSendFlags SendFlags = ComSendFlags::none);

	bool SendStringBuffer(const std::string StringMessage, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendJsonBuffer(const std::string JsonMessage, ComSendFlags SendFlags = ComSendFlags::none);

	template <typename T>
	bool RecvArray(TArray<T>& OutArrayData, size_t Size, ComRecvFlags RecvFlag = ComRecvFlags::none);
	bool RecvString(FString& OutStringMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);
	bool RecvJson(FString& OutJsonMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);

	bool RecvStringMultipart(TArray<FString>& OutMessages, ComRecvFlags RecvFlag = ComRecvFlags::none);

protected:
	zmq::context_t* Context;
	zmq::socket_t* SendSocket;
	zmq::socket_t* RecvSocket;
	
	zmq::poller_t<int>* Poller;

	std::string Address;
	int PollTimeout;
};
