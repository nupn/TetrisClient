#pragma once

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "..\Classes\ServerMessage.pb.h"
class ClientSocket;


struct MessageHeader
{
	google::protobuf::uint32 size;
	ServerMessage::MessageType type;
};
const int MessageHeaderSize = sizeof(MessageHeader);

class PacketHandler
{
public:
	PacketHandler() = default;
	virtual ~PacketHandler() = default;

	virtual void Handle(const ServerMessage::Login& message)
	{
	}
	virtual void Handle(const ServerMessage::Chat& message)
	{
	}
	virtual void Handle(const ServerMessage::Move& message)
	{
	}

	void SendChatMsg(std::string& chatMsg);
};
