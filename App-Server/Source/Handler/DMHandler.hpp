#pragma once

#include "ServerHandler.hpp"
#include "ServerPacket.hpp"

class RetrieveAllDMsHandler : public ServerHandler {
public:
    RetrieveAllDMsHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};