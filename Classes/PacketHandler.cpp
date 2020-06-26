#include "PacketHandler.h"
#include "CServerMgr.h"
#include "msg.pb.h"

PacketHandler::PacketHandler() {
	CServerMgr& svrs = boost::serialization::singleton<CServerMgr>::get_mutable_instance();
	svrs.AddHandler(ServerType::CHAT_SERVER, this);
}

PacketHandler::~PacketHandler() {
	CServerMgr& svrs = boost::serialization::singleton<CServerMgr>::get_mutable_instance();
	svrs.RemoveHandler(ServerType::CHAT_SERVER, nullptr);
}
