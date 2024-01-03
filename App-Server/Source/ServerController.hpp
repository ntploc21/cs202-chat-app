#pragma once
#include "Walnut/Networking/Server.h"
#include "Session.hpp"
#include "Handler/ServerHandler.hpp"
#include "ServerPacket.hpp"

class ServerController {
public:
    ServerController();
    ServerController(std::shared_ptr< Walnut::Server > server);
    
    void constructHandlers();

    void handlePacket(const Walnut::ClientInfo& client_info, const Walnut::Buffer buffer);
    void executeHandlers(const Walnut::ClientInfo& client_info,
                         const PacketType packet_type,
                         Walnut::BufferStreamReader& stream);

private:
    std::shared_ptr< Walnut::Server > m_server{};

    std::map< PacketType, std::vector< Handler::ID > > m_packet_handlers{};
};