#pragma once
#include <vector>
#include <memory>

#include <google/protobuf/io/coded_stream.h>
using namespace google::protobuf::io;

class PacketHandler
{
public:
	PacketHandler();
	virtual ~PacketHandler();

	virtual void Handle(int packetType, CodedInputStream* meesage) = 0;
};