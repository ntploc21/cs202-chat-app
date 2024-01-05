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

class UpdateLastSeenDMHandler : public ServerHandler {
public:
    UpdateLastSeenDMHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class AddPinMessageDMHandler : public ServerHandler {
public:
    AddPinMessageDMHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class RemovePinMessageDMHandler : public ServerHandler {
public:
    RemovePinMessageDMHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class DeleteMessageDMHandler : public ServerHandler {
public:
    DeleteMessageDMHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class UpdateNicknameDMHandler : public ServerHandler {
public:
    UpdateNicknameDMHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

void SendErrorDM(std::shared_ptr< Walnut::Server > server,
                 const Walnut::ClientInfo& client, PacketType packet_type,
                 std::string_view error_msg);