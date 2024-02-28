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
	std::vector<zmq::poller_event<int>> PollEvents;
	if (Poller->wait_all(PollEvents, std::chrono::milliseconds(PollTimeout)) > 0)
	{
		PollEvents.clear();
		return true;
	}

	PollEvents.clear();
	return false;
}

bool FSocketCommunicationServer::SendArray(const float* MessageData, size_t Size, zmq::send_flags SendFlags)
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

bool FSocketCommunicationServer::SendArray(const float MessageData[], zmq::send_flags SendFlags)
{
	auto Result = Socket->send(zmq::const_buffer(MessageData, sizeof MessageData), zmq::send_flags::dontwait);
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

bool FSocketCommunicationServer::SendArray(const TArray<float>& ArrayMessage, zmq::send_flags SendFlag)
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

bool FSocketCommunicationServer::SendString(const std::string StringMessage, zmq::send_flags SendFlags)
{
	return false;
}

bool FSocketCommunicationServer::SendJson(const std::string JsonMessage, zmq::send_flags SendFlags)
{
	return false;
}

bool FSocketCommunicationServer::RecvArray(float* MessageData, size_t Size)
{
	return false;
}

bool FSocketCommunicationServer::RecvString(std::string& StringMessage)
{
	return false;
}

bool FSocketCommunicationServer::RecvJson(std::string& JsonMessage)
{
	return false;
}
