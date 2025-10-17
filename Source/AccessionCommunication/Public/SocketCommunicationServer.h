// Copyright (C) HCI-BCU 2025. All rights reserved.

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
class ACCESSIONCOMMUNICATION_API FSocketCommunicationServer
{
public:
	FSocketCommunicationServer(const std::string SendAddress = "tcp://127.0.0.1:5555", const std::string RecvAddress = "tcp://127.0.0.1:5556", const int PollTimeout = 10);
	~FSocketCommunicationServer();

	/// <summary>
	/// Notifies when an Event Has Occured In the Socket.
	/// </summary>
	/// <returns>True, When an Event was Recived from the Socket. Otherwise False.</returns>
	bool EventOccured();

	/// <summary>
	/// Sends an Array of Data over the Socket, using a Buffer.
	/// </summary>
	/// <param name="MessageData">- The Array of Message Data To Send.</param>
	/// <param name="Size">- The Size of the Provided Data Array.</param>
	/// <param name="SendFlags">- The Send Flags for when sending over the socket.</param>
	/// <returns>True, if the Buffer was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayBuffer(const float *MessageData, size_t Size, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a Buffer.
	/// </summary>
	/// <param name="MessageData">- The Array of Message Data To Send.</param>
	/// <param name="SendFlags">- The Send Flags for when sending over the socket.</param>
	/// <returns>True, if the Buffer was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayBuffer(const float MessageData[], ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a Buffer.
	/// </summary>
	/// <param name="ArrayMessage">- The Array of Message Data To Send.</param>
	/// <param name="SendFlags">- The Send Flags for when sending over the socket.</param>
	/// <returns>True, if the Buffer was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayBuffer(const TArray<float> &ArrayMessage, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a message.
	/// </summary>
	/// <param name="MessageData">- The Array of Data To Send.</param>
	/// <param name="Size">- The Size of the Data in the Array.</param>
	/// <param name="SendFlags">- The Send Flags for when sending over the socket.</param>
	/// <returns>True, if the Message was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayMessage(const float *MessageData, size_t Size, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a message.
	/// </summary>
	/// <param name="MessageData">- The Array of Data To Send.</param>
	/// <param name="SendFlags">- The Send Flags To Use When Sending The Data.</param>
	/// <returns>True, if the Message was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayMessage(const float MessageData[], ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a message.
	/// </summary>
	/// <param name="ArrayMessage">- The Array of Data To Send.</param>
	/// <param name="SendFlags">- The Send Flags To Use When Sending The Data.</param>
	/// <returns>True, if the Message was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayMessage(const TArray<float> &ArrayMessage, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a message.
	/// </summary>
	/// <param name="MessageData">- The Array of Data To Send.</param>
	/// <param name="Size">- The Size of The Data Array.</param>
	/// <param name="Metadata">- The JSON Metadata to Send With The Message.</param>
	/// <param name="SendFlags">- The Send Flags To Use When Sending The Data.</param>
	/// <returns>True, if the Message was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayMessageWithMeta(const float *MessageData, size_t Size, const TSharedRef<FJsonObject> &Metadata, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a message.
	/// </summary>
	/// <param name="MessageData">- The Array of Data To Send.</param>
	/// <param name="Metadata">- The JSON Metadata to Send With The Message.</param>
	/// <param name="SendFlags">- The Send Flags To Use When Sending The Data.</param>
	/// <returns>True, if the Message was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayMessageWithMeta(const float MessageData[], const TSharedRef<FJsonObject> &Metadata, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends an Array of Data over the Socket, using a message.
	/// </summary>
	/// <param name="ArrayMessage">- The Array of Data To Send.</param>
	/// <param name="Metadata">- The JSON Metadata to Send With The Message.</param>
	/// <param name="SendFlags">- The Send Flags To Use When Sending The Data.</param>
	/// <returns>True, if the Message was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendArrayMessageWithMeta(const TArray<float> &ArrayMessage, const TSharedRef<FJsonObject> &Metadata, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends a String Buffer over the Socket.
	/// </summary>
	/// <param name="StringMessage">- The String Data To Send.</param>
	/// <param name="SendFlags">- The Send Flags To Use When Sending The Data.</param>
	/// <returns>True, if the Buffer was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendStringBuffer(const std::string StringMessage, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Sends a JSON Buffer over the Socket.
	/// </summary>
	/// <param name="JsonMessage">- The JSOn String Data To Send.</param>
	/// <param name="SendFlags">- The Send Flags To Use When Sending The Data.</param>
	/// <returns>True, if the Buffer was Sent Successfully. False, if an error occurs in sending.</returns>
	bool SendJsonBuffer(const std::string JsonMessage, ComSendFlags SendFlags = ComSendFlags::none);

	/// <summary>
	/// Recives an Array of Data from the Socket.
	/// </summary>
	/// <typeparam name="T">The Type To Cast The Recived Data In The Array To.</typeparam>
	/// <param name="OutArrayData">- The Returned Array of Data From The Socket.</param>
	/// <param name="Size">- The Size of the Data Recieved From The Array.</param>
	/// <param name="RecvFlag">- The Recv Flags To Use When Recieving The Data.</param>
	/// <returns>True, if the Data was Recived from the Socket Successfully. False, if an error occurs in receiving.</returns>
	template <typename T>
	bool RecvArray(TArray<T> &OutArrayData, size_t Size, ComRecvFlags RecvFlag = ComRecvFlags::none);

	/// <summary>
	/// Recives String Data From the Socket.
	/// </summary>
	/// <param name="OutStringMessage">- Returns the String Data Recived From the Socket.</param>
	/// <param name="RecvFlag">- The Recv Flags To Use When Recieving The Data.</param>
	/// <returns>True, if the Data was Recived from the Socket Successfully. False, if an error occurs in receiving.</returns>
	bool RecvString(FString &OutStringMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);

	/// <summary>
	/// Recieves JSON Data From The Socket.
	/// </summary>
	/// <param name="OutJsonMessage">- Returns the JSON String Data Recived From the Socket.</param>
	/// <param name="RecvFlag">- The Recv Flags To Use When Recieving The Data.</param>
	/// <returns>True, if the Data was Recived from the Socket Successfully. False, if an error occurd in receiving.</returns>
	bool RecvJson(FString &OutJsonMessage, ComRecvFlags RecvFlag = ComRecvFlags::none);

	/// <summary>
	/// Receives An Array of String Data From The Socket.
	/// </summary>
	/// <param name="OutMessages">- Returns the Multipart of String Data Received From the Socket.</param>
	/// <param name="RecvFlag">- The Recv Flags To Use When Recieving The Data.</param>
	/// <returns>True, if the Data was Received from the Socket Successfully. False, if an error occured in receiving.</returns>
	bool RecvStringMultipart(TArray<FString> &OutMessages, ComRecvFlags RecvFlag = ComRecvFlags::none);

	/// <summary>
	/// Receives An Array of String Data From The Socket, With JSON Metadata.
	/// </summary>
	/// <param name="OutMessages">- Returns the Received Array of String Data.</param>
	/// <param name="OutMetadata">- Returns a JSON Object containing Metadata.</param>
	/// <param name="RecvFlag">- The Recv Flags To Use When Recieving The Data.</param>
	/// <returns>True, if the Multipart was Received Successfully. False, if an error occured in receiving.</returns>
	bool RecvStringMultipartWithMeta(TArray<FString> &OutMessages, TSharedPtr<FJsonObject> &OutMetadata, ComRecvFlags RecvFlag = ComRecvFlags::none);

protected:
	/// <summary>
	/// Recieves a Multipart Message From The Socket, and a Metadata Object.
	/// </summary>
	/// <param name="OutMultipartMessages">- Returns the Array of Messages Contained in The Multipart.</param>
	/// <param name="OutMetadata">- Returns the Metadata JSON Object from the Multipart.</param>
	/// <param name="RecvFlags">- The Recv Flags To Use When Recieving The Data.</param>
	/// <returns>True, if the Multipart was Recieved Successfully. False, if an error occured in receiving.</returns>
	bool RecvMultipartWithMeta(std::vector<zmq::message_t> &OutMultipartMessages, TSharedPtr<FJsonObject> &OutMetadata, ComRecvFlags RecvFlags);

	/// <summary>
	/// Serializes the JSON Object into a JSON String.
	/// </summary>
	/// <param name="InJsonObject">- The JSON Object To Serialize.</param>
	/// <param name="OutJsonString">- The Returned Serialized JSON String from Serialization.</param>
	/// <returns>True, if the JSON Object was Successful in Serialization. Otherwise False.</returns>
	bool SerializeJSON(const TSharedRef<FJsonObject> &InJsonObject, FString &OutJsonString);

	/// <summary>
	/// Deserializes the JSON String into a JSON Object.
	/// </summary>
	/// <param name="InJsonString">- The JSON String To Deserialize.</param>
	/// <param name="OutJsonObject">- The Returned JSON Object from Deserialization.</param>
	/// <returns>True, if the JSON was Successfuly in Deserialization. Otherwise False.</returns>
	bool DeserializeJSON(const FString &InJsonString, TSharedPtr<FJsonObject> &OutJsonObject);

protected:
	/// <summary>
	/// The Context Used for the Socket Communication.
	/// </summary>
	zmq::context_t *Context;

	/// <summary>
	/// The Socket Used For Sending Data.
	/// </summary>
	zmq::socket_t *SendSocket;

	/// <summary>
	/// The Socket Used For Receiving Data.
	/// </summary>
	zmq::socket_t *RecvSocket;

	/// <summary>
	/// The Poller used for Polling for Events on the Receiving Socket.
	/// </summary>
	zmq::poller_t<int> *Poller;

	std::string SendAddress;
	std::string RecvAddress;

	/// <summary>
	/// The Time Taken By The Poller To Look For Events.
	/// </summary>
	int PollTimeout;
};
