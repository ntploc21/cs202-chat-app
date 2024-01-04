#pragma once

#include "ServerHandler.hpp"
#include "ServerPacket.hpp"

class RetrieveAllGroupsHandler : public ServerHandler {
public:
    RetrieveAllGroupsHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class SendGroupMessageHandler : public ServerHandler {
public:
	SendGroupMessageHandler();

private:
	void handleImpl(const Walnut::ClientInfo& client_info,
		const PacketType packet_type,
		Walnut::BufferStreamReader& stream) override;
};;