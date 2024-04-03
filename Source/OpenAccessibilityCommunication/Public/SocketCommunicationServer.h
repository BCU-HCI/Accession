// Copyright F-Dudley. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#ifdef WITH_ZEROMQ
#include "zmq.hpp"
#include "zmq_addon.hpp"
#else
#error "ZeroMQ Could not be found. Please Make Sure ZEROMQ is Installed Correctly, and the WITH_ZEROMQ Definition is Valid."
#endif // WITH_ZEROMQ

class FJsonObject;

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

	bool SendArrayMessageWithMeta(const float* MessageData, size_t Size, const TSharedRef<FJsonObject>& Metadata, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArrayMessageWithMeta(const float MessageData[], const TSharedRef<FJsonObject>& Metadata, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendArrayMessageWithMeta(const TArray<float>& ArrayMessage, const TSharedRef<FJsonObject>& Metadata, ComSendFlags SendFlags = ComSendFlags::none);

	bool SendStringBuffer(const std::string StringMessage, ComSendFlags SendFlags = ComSendFlags::none);
	bool SendJsonBuffer(const std::string JsonMessage, ComSendFlags SendFlags = ComSendFlags::none);

	template <typename T>
	bool RecvArray(TArray<T>& OutArrayData, size_t Size, ComRecvFlags RecvFlag = ComRecvFlags::none);
	bool RecvString(FString& OutStringMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);
	bool RecvJson(FString& OutJsonMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);

	bool RecvStringMultipart(TArray<FString>& OutMessages, ComRecvFlags RecvFlag = ComRecvFlags::none);
	bool RecvStringMultipartWithMeta(TArray<FString>& OutMessages, TSharedPtr<FJsonObject>& OutMetadata, ComRecvFlags RecvFlag = ComRecvFlags::none);

protected:

	bool RecvMultipartWithMeta(std::vector<zmq::message_t>& OutMultipartMessages, TSharedPtr<FJsonObject>& OutMetadata, ComRecvFlags RecvFlags);

	bool SerializeJSON(const TSharedRef<FJsonObject>& InJsonObject, FString& OutJsonString);
	bool DeserializeJSON(const FString& InJsonString, TSharedPtr<FJsonObject>& OutJsonObject);

protected:
	zmq::context_t* Context;
	zmq::socket_t* SendSocket;
	zmq::socket_t* RecvSocket;
	
	zmq::poller_t<int>* Poller;

	std::string Address;
	int PollTimeout;
};
