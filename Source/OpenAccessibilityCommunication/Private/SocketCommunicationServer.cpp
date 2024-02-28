// Copyright F-Dudley. All Rights Reserved.

#include "SocketCommunicationServer.h"

FSocketReqServer::FSocketReqServer(const std::string address = "tcp://127.0.0.1:5555")
{
}

FSocketReqServer::~FSocketReqServer()
{
	/*
	Socket->close();

	delete Socket;

	Context->shutdown();
	Context->close();

	zmq_ctx_destroy(Context);

	delete Context;
	*/
}
/*
bool FSocketReqServer::Send(const std::string Message)
{

	if (Message.empty())
	{
		return false;
	}

	zmq::const_buffer ReqMessage = zmq::buffer(Message);

	auto SendRes = Socket->send(ReqMessage, zmq::send_flags::none);
	if (SendRes.has_value())
	{

		// Wait for a response
		zmq::message_t Reply;
		auto RecvRes = Socket->recv(Reply, zmq::recv_flags::none);
		if (RecvRes.has_value())
		{
			UE_LOG(LogTemp, Warning, TEXT("Received: %s"), *FString(static_cast<char*>(Reply.data()), Reply.size()));

			return true;
		}

		return false;
	}

	return false;
}

bool FSocketReqServer::Send(const float* Message, size_t Size)
{
	if (Message == nullptr)
		return false;

	return false;
}

void FSocketReqServer::Tick(float DeltaTime)
{

}
*/