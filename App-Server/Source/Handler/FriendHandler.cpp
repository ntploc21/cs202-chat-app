#include "FriendHandler.hpp"

#include <iostream>

#include "../ClientManager.hpp"
#include "FriendManager.hpp"
#include "User-Utils.hpp"
#include "UserManager.hpp"
#include "Walnut/Serialization/BufferStream.h"

RetrieveAllUsersHandler::RetrieveAllUsersHandler() {}
RetrieveAllFriendsHandler::RetrieveAllFriendsHandler() {}
RetrieveAllGroupsHandler::RetrieveAllGroupsHandler() {}
AddFriendHandler::AddFriendHandler() {}
AcceptFriendHandler::AcceptFriendHandler() {}
DeclineFriendHandler::DeclineFriendHandler() {}
UnfriendHandler::UnfriendHandler() {}
RetrievePendingFriendRequestsHandler::RetrievePendingFriendRequestsHandler() {}

void RetrieveAllUsersHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                         const PacketType packet_type,
                                         Walnut::BufferStreamReader& stream) {
    SendAllUsers(client_info.ID, packet_type, stream, m_server);
}

void RetrieveAllFriendsHandler::handleImpl(
    const Walnut::ClientInfo& client_info, const PacketType packet_type,
    Walnut::BufferStreamReader& stream) {
    SendAllFriends(client_info.ID, packet_type, stream, m_server);
}

void RetrieveAllGroupsHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                          const PacketType packet_type,
                                          Walnut::BufferStreamReader& stream) {}

void AddFriendHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                  const PacketType packet_type,
                                  Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];
    int user_id = client.get_user_id();

    int friend_id;
    std::string note;
    stream.ReadRaw< int >(friend_id);
    stream.ReadString(note);

    if (FriendManager::getInstance().is_friend(user_id, friend_id)) {
        // send error ...

        return;
    }

    if (!FriendManager::getInstance()
             .add_friend(user_id, friend_id, note)
             .has_value()) {
        // send error ...

        return;
    }

    auto client_friend = ClientManager::getInstance().getClientID(friend_id);

    if (client_friend.has_value()) {
        SendPendingFriendRequests(client_friend.value(),
                                  PacketType::RetrievePendingFriendRequests,
                                  stream, m_server);
    }

    SendSuccess(client_info.ID, packet_type, stream, m_server);
}

void AcceptFriendHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                     const PacketType packet_type,
                                     Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];
    int user_id = client.get_user_id();

    int friend_id;
    stream.ReadRaw< int >(friend_id);

    if (!FriendManager::getInstance()
             .accept_friend(user_id, friend_id)
             .has_value()) {
        // send error ...
        return;
    }

    auto client_friend = ClientManager::getInstance().getClientID(friend_id);

    if (client_friend.has_value()) {
        SendPendingFriendRequests(client_friend.value(),
                                  PacketType::RetrievePendingFriendRequests,
                                  stream, m_server);
        SendAllFriends(client_friend.value(), PacketType::RetrieveAllFriends,
                       stream, m_server);
    }

    SendAllFriends(client_info.ID, PacketType::RetrieveAllFriends, stream,
                   m_server);
    SendPendingFriendRequests(client_info.ID,
                              PacketType::RetrievePendingFriendRequests, stream,
                              m_server);
    SendSuccess(client_info.ID, packet_type, stream, m_server);
}

void DeclineFriendHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                      const PacketType packet_type,
                                      Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int user_id = client.get_user_id();

    int friend_id;

    stream.ReadRaw< int >(friend_id);

    if (!FriendManager::getInstance().decline_friend(user_id, friend_id)) {
        // send error ...
        return;
    }

    auto client_friend = ClientManager::getInstance().getClientID(friend_id);

    if (client_friend.has_value()) {
        SendPendingFriendRequests(client_friend.value(),
                                  PacketType::RetrievePendingFriendRequests,
                                  stream, m_server);
    }

    SendSuccess(client_info.ID, packet_type, stream, m_server);
}

void UnfriendHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                 const PacketType packet_type,
                                 Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int user_id = client.get_user_id();

    int friend_id;

    stream.ReadRaw< int >(friend_id);

    if (!FriendManager::getInstance().unfriend(user_id, friend_id)) {
        // send error ...
        return;
    }

    auto client_friend = ClientManager::getInstance().getClientID(friend_id);

    if (client_friend.has_value()) {
        SendAllFriends(client_friend.value(), PacketType::RetrieveAllFriends,
                       stream, m_server);
    }

    SendAllFriends(client_info.ID, PacketType::RetrieveAllFriends, stream,
                   m_server);

    SendSuccess(client_info.ID, packet_type, stream, m_server);
}

void RetrievePendingFriendRequestsHandler::handleImpl(
    const Walnut::ClientInfo& client_info, const PacketType packet_type,
    Walnut::BufferStreamReader& stream) {
    SendPendingFriendRequests(client_info.ID, packet_type, stream, m_server);
}

void SendAllUsers(Walnut::ClientID clientID, const PacketType packet_type,
                  Walnut::BufferStreamReader& stream,
                  std::shared_ptr< Walnut::Server > server) {
    auto users = UserManager::getInstance().get_users();
    filter_credentials(users);

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(8192);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteArray(users);

    server->SendBufferToClient(
        clientID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}

void SendAllFriends(Walnut::ClientID clientID, const PacketType packet_type,
                    Walnut::BufferStreamReader& stream,
                    std::shared_ptr< Walnut::Server > server) {
    auto& client = ClientManager::getInstance()[clientID];
    int user_id = client.get_user_id();

    auto friends = FriendManager::getInstance().get_friends(user_id);

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(8192);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteArray(friends);

    server->SendBufferToClient(
        clientID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}

void SendPendingFriendRequests(Walnut::ClientID clientID,
                               const PacketType packet_type,
                               Walnut::BufferStreamReader& stream,
                               std::shared_ptr< Walnut::Server > server) {
    auto& client = ClientManager::getInstance()[clientID];
    int user_id = client.get_user_id();

    auto pending_friend_requests =
        FriendManager::getInstance().get_friend_requests(user_id);

    std::vector< User > users{};
    std::vector< std::string > notes{};

    for (auto& friend_request : pending_friend_requests) {
        auto user =
            UserManager::getInstance().get_user(friend_request.get_user_1_id());
        if (user.has_value()) {
            users.push_back(user.value());
        }
        notes.push_back(friend_request.get_note());
    }

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(8192);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteArray(users);
    out_stream.WriteArray(notes);

    server->SendBufferToClient(
        clientID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}

void SendSuccess(Walnut::ClientID clientID, const PacketType packet_type,
                 Walnut::BufferStreamReader& stream,
                 std::shared_ptr< Walnut::Server > server) {
    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(8192);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);

    server->SendBufferToClient(
        clientID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}
