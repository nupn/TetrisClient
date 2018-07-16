#include "PacketHandler.h"
#include "NetworkThread.h"
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include "ServerMessage.pb.h"


using namespace google;

void PacketHandler::SendChatMsg(std::string& chatMsg)
{
	ServerMessage::Chat sendMessage;
	sendMessage.set_dst_id(10);
	sendMessage.set_name("aas");
	sendMessage.set_message(chatMsg);

	MessageHeader messageHeader;
	messageHeader.size = sendMessage.ByteSize();
	messageHeader.type = ServerMessage::CHAT;

}