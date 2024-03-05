// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "zmq.hpp"
#include "zmq_addon.hpp"

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FSocketCommunicationServer
{
public:
	using ComSendFlags = zmq::send_flags;
	using ComRecvFlags = zmq::recv_flags;

	FSocketCommunicationServer(const std::string Address, const int Timeout);
	~FSocketCommunicationServer();

	bool EventOccured();

	bool SendArray(const float* MessageData, size_t Size, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArray(const float MessageData[], ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArray(const TArray<float>& ArrayMessage, ComSendFlags SendFlags = ComSendFlags::none);

	bool SendString(const std::string StringMessage, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendJson(const std::string JsonMessage, ComSendFlags SendFlags = ComSendFlags::none);

	template <typename T>
	bool RecvArray(TArray<T>& OutArrayData, size_t Size, ComRecvFlags RecvFlag = ComRecvFlags::none);
	bool RecvString(FString& OutStringMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);
	bool RecvJson(FString& OutJsonMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);

	bool RecvStringMultipart(TArray<FString>& OutMessages, ComRecvFlags RecvFlag = ComRecvFlags::none);

protected:
	zmq::context_t* Context;
	zmq::socket_t* Socket;
	
	zmq::poller_t<int>* Poller;

	std::string Address;
	int PollTimeout;
};
