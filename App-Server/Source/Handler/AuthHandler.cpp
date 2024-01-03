#include "AuthHandler.hpp"

#include <iostream>

#include "../ClientManager.hpp"
#include "Authenticator.hpp"
#include "UserManager.hpp"
#include "Walnut/Serialization/BufferStream.h"

AuthHandler::AuthHandler() {}
ClientRegisterHandler::ClientRegisterHandler() {}
ClientLoginHandler::ClientLoginHandler() {}
ClientLogoutHandler::ClientLogoutHandler() {}

void AuthHandler::handleImpl(const Walnut::ClientInfo& client_info,
                             const PacketType packet_type,
                             Walnut::BufferStreamReader& stream) {
    // check if client is authenticated

    auto& client = ClientManager::getInstance()[client_info.ID];

    if (!Authenticator::getInstance().validate_session(
            client.get_session_id())) {
        SendError(m_server, client_info, packet_type,
                  "Client is not authenticated!");
        return;
    }
}

void ClientRegisterHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                       const PacketType packet_type,
                                       Walnut::BufferStreamReader& stream) {
    std::string username;
    std::string password;
    std::string fullname;

    if (!stream.ReadString(username) || !stream.ReadString(password) ||
        !stream.ReadString(fullname)) {
        SendError(m_server, client_info, packet_type,
                  "Username, password and fullname must not be empty!");
        return;
    }

    auto user = UserManager::getInstance().findByUsernameAndPassword(username,
                                                                     password);

    if (user.has_value()) {
        SendError(m_server, client_info, packet_type, "User already exists");
        return;
    }
    User new_user(username, password, fullname);
    int user_id = UserManager::getInstance().add_user(new_user);

    new_user = UserManager::getInstance().get_user(user_id).value();

    auto& client = ClientManager::getInstance()[client_info.ID];
    client = Authenticator::getInstance().add_session(user_id);

    SendClientConnectionSuccess(m_server, client_info, packet_type, new_user);

    // broadcast to all clients
    auto clients = ClientManager::getInstance().getAllClients();

    for (auto client_id : clients) {
        if (client_id == client_info.ID) continue;

        Walnut::Buffer scratch_buffer{};
        scratch_buffer.Allocate(8192);

        Walnut::BufferStreamWriter out_stream(scratch_buffer);
        out_stream.WriteRaw< PacketType >(PacketType::RetrieveAllUsers);

        auto users = UserManager::getInstance().get_users();

        out_stream.WriteRaw< bool >(true);
        out_stream.WriteArray(users);

        m_server->SendBufferToClient(
            client_id,
            Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
    }
}

void ClientLoginHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                    const PacketType packet_type,
                                    Walnut::BufferStreamReader& stream) {
    std::string username_login;
    std::string password_login;

    if (!stream.ReadString(username_login) ||
        !stream.ReadString(password_login)) {
        SendError(m_server, client_info, packet_type,
                  "Username and password must not be empty!");
        return;
    }

    auto user_login = UserManager::getInstance().findByUsernameAndPassword(
        username_login, password_login);

    if (user_login.has_value()) {
        int user_id = user_login.value().get_user_id();
        auto& client = ClientManager::getInstance()[client_info.ID];
        client = Authenticator::getInstance().add_session(user_id);

        SendClientConnectionSuccess(m_server, client_info, packet_type,
                                    user_login.value());
    } else {
        SendError(m_server, client_info, packet_type,
                  "Login credentials incorrect!");
    }
}

void ClientLogoutHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                     const PacketType packet_type,
                                     Walnut::BufferStreamReader& stream) {
    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}

void SendError(std::shared_ptr< Walnut::Server > server,
               const Walnut::ClientInfo& client, PacketType packet_type,
               std::string_view error_msg) {
    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);
    stream.WriteRaw< PacketType >(packet_type);
    stream.WriteRaw< bool >(false);
    stream.WriteString(error_msg);

    server->SendBufferToClient(
        client.ID, Walnut::Buffer(scratch_buffer, stream.GetStreamPosition()));
}

void SendClientConnectionSuccess(std::shared_ptr< Walnut::Server > server,
                                 const Walnut::ClientInfo& client,
                                 PacketType packet_type, User user) {
    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);
    stream.WriteRaw< PacketType >(packet_type);
    stream.WriteRaw< bool >(true);
    stream.WriteObject(user);

    server->SendBufferToClient(
        client.ID, Walnut::Buffer(scratch_buffer, stream.GetStreamPosition()));

    // console.AddItalicMessage("Info", "Client {} connected",
    //                            client.ConnectionDesc);
}