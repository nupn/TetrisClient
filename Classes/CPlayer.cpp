#include "CPlayer.h"
#include "protocol.h"
#include "msg.pb.h"
#include "CServerMgr.h"

void CPlayer::Login()
{
	char sendbuf[1024];
	
	game::c_login loginPacket;
	int size = loginPacket.ByteSizeLong() + 8;
	
	google::protobuf::io::ArrayOutputStream aos(sendbuf, size);
	CodedOutputStream* coded_output = new CodedOutputStream(&aos);
	coded_output->WriteVarint32(C_LOGIN);
	coded_output->WriteVarint32(loginPacket.ByteSizeLong());
	loginPacket.SerializeToCodedStream(coded_output);

	CServerMgr& svrs = boost::serialization::singleton<CServerMgr>::get_mutable_instance();
	svrs.SendPacket(ServerType::CHAT_SERVER, sendbuf, size);
}

void CPlayer::Handle(int packetType, CodedInputStream* meesage)
{
	switch (packetType)
	{
	case S_LOGIN:
	{
		game::s_login payload;
		google::protobuf::io::CodedInputStream::Limit msgLimit = meesage->PushLimit(1024);
		if (!payload.ParseFromCodedStream(meesage)) {
			printf("        parseFromCodedStream Fail\n");
		}
		meesage->PopLimit(1024);

		m_nId = payload.id();
		m_strName = payload.name();
		_isLogin = true;
	}
	default:
		break;
	}
}

bool CPlayer::IsLogin()
{
	return _isLogin;
}

std::string CPlayer::Name() const
{
	return m_strName;
}

int CPlayer::Id() const
{
	return m_nId;
}