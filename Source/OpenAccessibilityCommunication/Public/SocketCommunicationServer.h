// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if WITH_ZEROMQ
#include "zmq.hpp"
#include "zmq_addon.hpp"
#endif


UENUM(BlueprintType)
enum class MessageBodyType : uint8
{
	OA_Undefined = 0,
	OA_Transcription = 1,
};

/**
 * 
 */
class OPENACCESSIBILITYCOMMUNICATION_API FSocketReqServer
{
public:
	FSocketReqServer(std::string Address);
	~FSocketReqServer();

	/*
	bool Send(const std::string Message);
	bool Send(const float* MessageData, size_t Size);

protected:
	void Tick(float DeltaTime);

protected:
	zmq::context_t* Context;

	zmq::socket_t* Socket;
	*/
};
