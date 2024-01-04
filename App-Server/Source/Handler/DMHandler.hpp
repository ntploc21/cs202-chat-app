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

class SendDirectMessageHandler : public ServerHandler {
public:
    SendDirectMessageHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

void SendErrorDM(std::shared_ptr< Walnut::Server > server,
               const Walnut::ClientInfo& client, PacketType packet_type,
               std::string_view error_msg);