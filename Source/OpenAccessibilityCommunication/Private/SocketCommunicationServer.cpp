// Copyright F-Dudley. All Rights Reserved.

#include "SocketCommunicationServer.h"
#include "OpenAccessibilityComLogging.h"

FSocketCommunicationServer::FSocketCommunicationServer(const std::string Address = "tcp://127.0.0.1:5555", const int PollTimeout = 10) 
	: Address(Address), PollTimeout(PollTimeout)
{
	Context = new zmq::context_t();
	if (Context == nullptr)
	{
		UE_LOG(LogOpenAccessibilityCom, Error, TEXT("Failed to create ZMQ context"));
		return;
	}

	Socket = new zmq::socket_t(*Context, ZMQ_REQ);
	if (Socket == nullptr)
	{
		UE_LOG(LogOpenAccessibilityCom, Error, TEXT("Failed to create ZMQ socket"));
		return;
	}
	
	Poller = new zmq::poller_t<int>();
	if (Poller == nullptr)
	{
		UE_LOG(LogOpenAccessibilityCom, Error, TEXT("Failed to create ZMQ poller"));
		return;
	}

	Socket->connect(Address);
	Poller->add(*Socket, zmq::event_flags::pollin);
}

FSocketCommunicationServer::~FSocketCommunicationServer()
{
	Poller->remove(*Socket);
	delete Poller; Poller = nullptr;

	Socket->disconnect(Address);
	Socket->close();
	delete Socket; Socket = nullptr;

	Context->shutdown();
	Context->close();
	delete Context; Context = nullptr;
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

bool FSocketCommunicationServer::SendArrayBuffer(const float* MessageData, size_t Size, ComSendFlags SendFlags)
{
	auto Result = Socket->send(zmq::const_buffer(MessageData, Size * sizeof(float)), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), Size * sizeof(float));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayBuffer(const float MessageData[], ComSendFlags SendFlags)
{
	auto Result = Socket->send(zmq::const_buffer(MessageData, sizeof MessageData), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(sizeof MessageData));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayBuffer(const TArray<float>& ArrayMessage, ComSendFlags SendFlag)
{
	auto Result = Socket->send(zmq::const_buffer(ArrayMessage.GetData(), ArrayMessage.Num() * sizeof(float)), SendFlag);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(ArrayMessage.Num() * sizeof(float)));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessage(const float* MessageData, size_t Size, ComSendFlags SendFlags)
{
	auto Result = Socket->send(zmq::message_t(MessageData, Size * sizeof(float)), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), Size * sizeof(float));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessage(const float MessageData[], ComSendFlags SendFlags)
{
	auto Result = Socket->send(zmq::message_t(MessageData, sizeof MessageData), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(sizeof MessageData));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendArrayMessage(const TArray<float>& ArrayMessage, ComSendFlags SendFlags)
{
	auto Result = Socket->send(zmq::message_t(ArrayMessage.GetData(), ArrayMessage.Num() * sizeof(float)), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent Array || Sent %d of %d bytes"), Result.value(), int(ArrayMessage.Num() * sizeof(float)));
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendStringBuffer(const std::string StringMessage, ComSendFlags SendFlags)
{
	auto Result = Socket->send(zmq::const_buffer(StringMessage.c_str(), StringMessage.size()), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent String || Sent %d of %d bytes"), Result.value(), StringMessage.size());
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent String || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::SendJsonBuffer(const std::string JsonMessage, ComSendFlags SendFlags)
{
	auto Result = Socket->send(zmq::const_buffer(JsonMessage.c_str(), JsonMessage.size()), SendFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Sent JSON || Sent %d of %d bytes"), Result.value(), JsonMessage.size());
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Sent JSON || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}



template <typename T> 
bool FSocketCommunicationServer::RecvArray(TArray<T>& OutArrayData, size_t Size, ComRecvFlags RecvFlags)
{
	zmq::message_t RecvMessage;

	auto Result = Socket->recv(RecvMessage, RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Recv Array || Recv %d bytes"), Result.value());
		
		OutArrayData.Append(RecvMessage.data<T>(), Result.value());
		
		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Recv Array || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::RecvString(FString& OutStringMessage, ComRecvFlags RecvFlags)
{
	zmq::message_t RecvMessage;

	auto Result = Socket->recv(RecvMessage, RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Recv String || Recv %d bytes"), Result.value());

		OutStringMessage = FString(Result.value(), UTF8_TO_TCHAR(RecvMessage.data()));

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{

		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Recv String || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::RecvJson(FString& OutJsonMessage, ComRecvFlags RecvFlags)
{
	zmq::message_t RecvMessage;

	auto Result = Socket->recv(RecvMessage, RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Recv JSON || Recv %d bytes"), Result.value());

		OutJsonMessage = FString(Result.value(), UTF8_TO_TCHAR(RecvMessage.data()));

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Recv JSON || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}

bool FSocketCommunicationServer::RecvStringMultipart(TArray<FString>& OutMessages, ComRecvFlags RecvFlags)
{
	std::vector<zmq::message_t> RecvMessages;

	auto Result = zmq::recv_multipart(*Socket, std::back_inserter(RecvMessages), RecvFlags);
	if (Result.has_value())
	{
		UE_LOG(LogOpenAccessibilityCom, Log, TEXT("|| Com Server: Recv Multipart || Recv %d messages"), Result.value());

		for (auto& Message : RecvMessages)
		{
			OutMessages.Add(FString(Message.size(), UTF8_TO_TCHAR(Message.data())));
		}

		return true;
	}
	else if (zmq_errno() == EAGAIN)
	{
		UE_LOG(LogOpenAccessibilityCom, Warning, TEXT("|| Com Server: Recv Multipart || EAGAIN Error Occured ||"));
		return true;
	}

	return false;
}