#pragma once

#include "ServerHandler.hpp"
#include "ServerPacket.hpp"
#include "User.hpp"

class AuthHandler : public ServerHandler {
public:
    AuthHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class ClientRegisterHandler : public ServerHandler {
public:
    ClientRegisterHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class ClientLoginHandler : public ServerHandler {
public:
    ClientLoginHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class ClientLogoutHandler : public ServerHandler {
public:
    ClientLogoutHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

void SendError(std::shared_ptr< Walnut::Server > server,
               const Walnut::ClientInfo& client, PacketType packet_type,
               std::string_view error_msg);
void SendClientConnectionSuccess(std::shared_ptr< Walnut::Server > server,
                                 const Walnut::ClientInfo& client,
                                 PacketType packet_type, User user);
