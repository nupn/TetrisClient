#pragma once
#include "PacketHandler.h"
#include <boost/serialization/singleton.hpp>
#include <string>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
using namespace google::protobuf::io;

class CPlayer : public boost::serialization::singleton<CPlayer>, PacketHandler
{
public:
	friend class boost::serialization::singleton<CPlayer>;

	static CPlayer& GetMutable() {
		return boost::serialization::singleton<CPlayer>::get_mutable_instance();
	}

public:
	void Login();
	bool IsLogin();
	std::string Name() const;

	int Id() const;
	void Handle(int packetType, CodedInputStream* meesage);

private:
	std::string m_strName;
	int m_nId;
	bool _isLogin = false;
};

