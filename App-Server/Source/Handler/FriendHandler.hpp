#pragma once

#include "ServerHandler.hpp"
#include "ServerPacket.hpp"
#include "User.hpp"

class RetrieveAllUsersHandler : public ServerHandler {
public:
    RetrieveAllUsersHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class RetrieveAllFriendsHandler : public ServerHandler {
public:
    RetrieveAllFriendsHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class RetrieveAllGroupsHandler : public ServerHandler {
public:
    RetrieveAllGroupsHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class AddFriendHandler : public ServerHandler {
public:
    AddFriendHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class AcceptFriendHandler : public ServerHandler {
public:
    AcceptFriendHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class DeclineFriendHandler : public ServerHandler {
public:
    DeclineFriendHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class UnfriendHandler : public ServerHandler {
public:
    UnfriendHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

class RetrievePendingFriendRequestsHandler : public ServerHandler {
public:
    RetrievePendingFriendRequestsHandler();

private:
    void handleImpl(const Walnut::ClientInfo& client_info,
                    const PacketType packet_type,
                    Walnut::BufferStreamReader& stream) override;
};

void SendAllUsers(Walnut::ClientID clientID, const PacketType packet_type,
                  Walnut::BufferStreamReader& stream,
                  std::shared_ptr< Walnut::Server > server);

void SendAllFriends(Walnut::ClientID clientID, const PacketType packet_type,
                    Walnut::BufferStreamReader& stream,
                    std::shared_ptr< Walnut::Server > server);

void SendPendingFriendRequests(Walnut::ClientID clientID,
                               const PacketType packet_type,
                               Walnut::BufferStreamReader& stream,
                               std::shared_ptr< Walnut::Server > server);

void SendSuccess(Walnut::ClientID clientID, const PacketType packet_type,
                 Walnut::BufferStreamReader& stream,
                 std::shared_ptr< Walnut::Server > server);