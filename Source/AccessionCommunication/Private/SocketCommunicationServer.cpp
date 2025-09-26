// Copyright (C) HCI-BCU. All rights reserved.
// Published under GPLv3 License, 2025. See LICENSE in the Plugin Root for more information.

#include "SocketCommunicationServer.h"
#include "AccessionComLogging.h"

#include "Serialization/JsonSerializer.h"

FSocketCommunicationServer::FSocketCommunicationServer(const std::string SendAddress, std::string RecvAddress, const int PollTimeout)
	: SendAddress(SendAddress), RecvAddress(RecvAddress), PollTimeout(PollTimeout)
{
	Context = new zmq::context_t(1);
	if (Context == nullptr)
	{
		UE_LOG(LogAccessionCom, Error, TEXT("Failed to create ZMQ context"));
		return;
	}

	SendSocket = new zmq::socket_t(*Context, ZMQ_PUSH);
	if (SendSocket == nullptr)
	{
		UE_LOG(LogAccessionCom, Error, TEXT("Failed to create ZMQ socket"));
		return;
	}

	RecvSocket = new zmq::socket_t(*Context, ZMQ_PULL);
	if (RecvSocket == nullptr)
	{
		UE_LOG(LogAccessionCom, Error, TEXT("Failed to create ZMQ socket"));
		return;
	}

	Poller = new zmq::poller_t<int>();
	if (Poller == nullptr)
	{
		UE_LOG(LogAccessionCom, Error, TEXT("Failed to create ZMQ poller"));
		return;
	}

	SendSocket->connect(SendAddress);
	RecvSocket->bind(RecvAddress);

	Poller->add(*RecvSocket, zmq::event_flags::pollin);
}

FSocketCommunicationServer::~FSocketCommunicationServer()
{
	Poller->remove(*RecvSocket);
	delete Poller;
	Poller = nullptr;

	SendSocket->disconnect(SendAddress);
	SendSocket->close();
	delete SendSocket;
	SendSocket = nullptr;

	RecvSocket->unbind(RecvAddress);
	RecvSocket->close();
	delete RecvSocket;
	RecvSocket = nullptr;

	Context->shutdown();
	Context->close();
	delete Context;
	Context = nullptr;
}

bool FSocketCommunicationServer::EventOccured()
{
	std::vector<zmq::poller_event<int>> PollEvents(1);
	if (Poller->wait_all(PollEvents, std::chrono::milliseconds(PollTimeout)) > 0)
	{
		PollEvents.clear();
		return true;
	}

	PollEvents.clear();
	return false;
}

bool FSocketCommunicationServer::SendArrayBuffer(const float *MessageData, size_t Size, ComSendFlags SendFlags)
{
	auto Result = SendSocket->send(zmq::const_buffer(MessageData, Size * sizeof(float)), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), Size * sizeof(float));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayBuffer(const float MessageData[], ComSendFlags SendFlags)
{
	auto Result = SendSocket->send(zmq::const_buffer(MessageData, sizeof MessageData), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(sizeof MessageData));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayBuffer(const TArray<float> &ArrayMessage, ComSendFlags SendFlag)
{
	auto Result = SendSocket->send(zmq::const_buffer(ArrayMessage.GetData(), ArrayMessage.Num() * sizeof(float)), SendFlag);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(ArrayMessage.Num() * sizeof(float)));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessage(const float *MessageData, size_t Size, ComSendFlags SendFlags)
{
	auto Result = SendSocket->send(zmq::message_t(MessageData, Size * sizeof(float)), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), Size * sizeof(float));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessage(const float MessageData[], ComSendFlags SendFlags)
{
	auto Result = SendSocket->send(zmq::message_t(MessageData, sizeof MessageData), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(sizeof MessageData));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessage(const TArray<float> &ArrayMessage, ComSendFlags SendFlags)
{
	auto Result = SendSocket->send(zmq::message_t(ArrayMessage.GetData(), ArrayMessage.Num() * sizeof(float)), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(ArrayMessage.Num() * sizeof(float)));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessageWithMeta(const float *MessageData, size_t Size, const TSharedRef<FJsonObject> &Metadata, ComSendFlags SendFlags)
{
	FString MetaDataString;
	if (!SerializeJSON(Metadata, MetaDataString))
	{
		UE_LOG(LogAccessionCom, Error, TEXT("|| Com Server: Sent Array || Failed to serialize metadata ||"));
		return false;
	}

	std::vector<zmq::message_t> Messages;
	Messages.push_back(zmq::message_t(*MetaDataString, MetaDataString.Len() * sizeof(TCHAR)));
	Messages.push_back(zmq::message_t(MessageData, Size * sizeof(float)));

	auto Result = zmq::send_multipart(*SendSocket, Messages, SendFlags);

	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), Size * sizeof(float));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessageWithMeta(const float MessageData[], const TSharedRef<FJsonObject> &Metadata, ComSendFlags SendFlags)
{
	FString MetaDataString;
	if (!SerializeJSON(Metadata, MetaDataString))
	{
		UE_LOG(LogAccessionCom, Error, TEXT("|| Com Server: Sent Array || Failed to serialize metadata ||"));
		return false;
	}

	std::vector<zmq::message_t> Messages;
	Messages.push_back(zmq::message_t(*MetaDataString, MetaDataString.Len() * sizeof(TCHAR)));
	Messages.push_back(zmq::message_t(MessageData, sizeof MessageData));

	auto Result = zmq::send_multipart(*SendSocket, Messages, SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(sizeof MessageData));

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessageWithMeta(const TArray<float> &ArrayMessage, const TSharedRef<FJsonObject> &Metadata, ComSendFlags SendFlags)
{
	FString MetaDataString;
	if (!SerializeJSON(Metadata, MetaDataString))
	{
		UE_LOG(LogAccessionCom, Error, TEXT("|| Com Server: Sent Array || Failed to serialize metadata ||"));
		return false;
	}

	std::vector<zmq::message_t> Messages;
	Messages.push_back(zmq::message_t(*MetaDataString, MetaDataString.Len() * sizeof(TCHAR)));
	Messages.push_back(zmq::message_t(ArrayMessage.GetData(), ArrayMessage.Num() * sizeof(float)));

	auto Result = zmq::send_multipart(*SendSocket, Messages, SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d Messages"), Result.value(), Messages.size());

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));

		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendStringBuffer(const std::string StringMessage, ComSendFlags SendFlags)
{
	auto Result = SendSocket->send(zmq::const_buffer(StringMessage.c_str(), StringMessage.size()), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent String || Sent %d of %d bytes"), Result.value(), StringMessage.size());
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent String || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendJsonBuffer(const std::string JsonMessage, ComSendFlags SendFlags)
{
	auto Result = SendSocket->send(zmq::const_buffer(JsonMessage.c_str(), JsonMessage.size()), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Sent JSON || Sent %d of %d bytes"), Result.value(), JsonMessage.size());
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Sent JSON || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

template <typename T>
bool FSocketCommunicationServer::RecvArray(TArray<T> &OutArrayData, size_t Size, ComRecvFlags RecvFlags)
{
	zmq::message_t RecvMessage;

	auto Result = RecvSocket->recv(RecvMessage, RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Recv Array || Recv %d bytes"), Result.value());

		OutArrayData.Append(RecvMessage.data<T>(), Result.value());

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Recv Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::RecvString(FString &OutStringMessage, ComRecvFlags RecvFlags)
{
	zmq::message_t RecvMessage;

	auto Result = RecvSocket->recv(RecvMessage, RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Recv String || Recv %d bytes"), Result.value());

		OutStringMessage = FString(Result.value(), UTF8_TO_TCHAR(RecvMessage.data()));

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{

		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Recv String || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::RecvJson(FString &OutJsonMessage, ComRecvFlags RecvFlags)
{
	zmq::message_t RecvMessage;

	auto Result = RecvSocket->recv(RecvMessage, RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Recv JSON || Recv %d bytes"), Result.value());

		OutJsonMessage = FString(Result.value(), UTF8_TO_TCHAR(RecvMessage.data()));

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Recv JSON || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::RecvStringMultipart(TArray<FString> &OutMessages, ComRecvFlags RecvFlags)
{
	std::vector<zmq::message_t> RecvMessages;

	auto Result = zmq::recv_multipart(*RecvSocket, std::back_inserter(RecvMessages), RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Recv Multipart || Recv %d messages"), Result.value());

		for (auto &Message : RecvMessages)
		{
			OutMessages.Add(FString(Message.size(), UTF8_TO_TCHAR(Message.data())));
		}

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Recv Multipart || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::RecvStringMultipartWithMeta(TArray<FString> &OutMessages, TSharedPtr<FJsonObject> &OutMetadata, ComRecvFlags RecvFlag)
{
	std::vector<zmq::message_t> RecvMessages;
	if (!RecvMultipartWithMeta(RecvMessages, OutMetadata, RecvFlag))
		return false;

	for (auto &Message : RecvMessages)
	{
		OutMessages.Add(FString(Message.size(), UTF8_TO_TCHAR(Message.data())));
	}

	return true;
}

bool FSocketCommunicationServer::RecvMultipartWithMeta(std::vector<zmq::message_t> &OutMultipartMessages, TSharedPtr<FJsonObject> &OutMetadata, ComRecvFlags RecvFlags)
{
	auto Result = zmq::recv_multipart(*RecvSocket, std::back_inserter(OutMultipartMessages), RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogAccessionCom, Log, TEXT("|| Com Server: Recv Multipart || Recv %d messages"), Result.value());

		// Pop Metadata Messages from the Front of Array.
		zmq::message_t MetadataMessage = MoveTempIfPossible(OutMultipartMessages[0]);
		OutMultipartMessages.erase(OutMultipartMessages.begin());

		if (DeserializeJSON(FString(UTF8_TO_TCHAR(MetadataMessage.data()), MetadataMessage.size()), OutMetadata))
		{
			return true;
		}
		else
		{
			UE_LOG(LogAccessionCom, Error, TEXT("|| Com Server: Recv Multipart || Failed to deserialize metadata ||"));
			return false;
		}
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogAccessionCom, Warning, TEXT("|| Com Server: Recv Multipart || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SerializeJSON(const TSharedRef<FJsonObject> &InJsonObject, FString &OutJsonString)
{
	return FJsonSerializer::Serialize(InJsonObject, TJsonWriterFactory<TCHAR>::Create(&OutJsonString));
}

bool FSocketCommunicationServer::DeserializeJSON(const FString &InJsonString, TSharedPtr<FJsonObject> &OutJsonObject)
{
	return FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(InJsonString), OutJsonObject);
}
