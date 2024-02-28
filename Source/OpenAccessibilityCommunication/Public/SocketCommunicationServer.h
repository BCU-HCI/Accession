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
	FSocketCommunicationServer(const std::string Address, const int Timeout);
	~FSocketCommunicationServer();

	bool EventOccured();

	bool SendArray(const float* MessageData, size_t Size, zmq::send_flags SendFlags);
	bool SendArray(const float MessageData[], zmq::send_flags SendFlags);
	bool SendArray(const TArray<float>& ArrayMessage, zmq::send_flags SendFlags);

	bool SendString(const std::string StringMessage, zmq::send_flags SendFlags);
	bool SendJson(const std::string JsonMessage, zmq::send_flags SendFlags);

	
	bool RecvArray(float* MessageData, size_t Size);
	bool RecvString(std::string& StringMessage);
	bool RecvJson(std::string& JsonMessage);

protected:
	zmq::context_t* Context;
	zmq::socket_t* Socket;
	
	zmq::poller_t<int>* Poller;

	std::string Address;
	int PollTimeout;
};
