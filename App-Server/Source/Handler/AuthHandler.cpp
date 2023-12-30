#include "AuthHandler.hpp"
#include <iostream>
#include "Walnut/Serialization/BufferStream.h"
#include "UserManager.hpp"
#include "../ClientManager.hpp"
#include "Authenticator.hpp"

AuthHandler::AuthHandler() {}
ClientRegisterHandler::ClientRegisterHandler() {}
ClientLoginHandler::ClientLoginHandler() {}
ClientLogoutHandler::ClientLogoutHandler() {}

void AuthHandler::handleImpl(const Walnut::ClientInfo& client_info,
                             Walnut::BufferStreamReader& stream) {
    //std::cout << "AuthHandler::handleImpl" << std::endl;
}


void ClientRegisterHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                       Walnut::BufferStreamReader& stream) {

    std::string username;
    std::string password;
    std::string fullname;

    



    if (!stream.ReadString(username) || !stream.ReadString(password) ||
        !stream.ReadString(fullname)) {
        SendError(m_server, 
            client_info, PacketType::ClientRegisterRequest,
                  "Username, password and fullname must not be empty!");
        return;
    }

    auto user = UserManager::getInstance().findByUsernameAndPassword(username,
                                                                     password);

    if (user.has_value()) {
        SendError(m_server, client_info, PacketType::ClientRegisterRequest,
                  "User already exists");
    } else {
        User new_user(username, password, fullname);
        int user_id = UserManager::getInstance().add_user(new_user);

        auto& client = ClientManager::getInstance()[client_info.ID];
        client = Authenticator::getInstance().add_session(user_id);

        SendClientConnectionSuccess(m_server, client_info,
                                    PacketType::ClientRegisterRequest, client);
    }
}


void ClientLoginHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                    Walnut::BufferStreamReader& stream) {

    std::string username_login;
    std::string password_login;

    if (!stream.ReadString(username_login) ||
        !stream.ReadString(password_login)) {
        SendError(m_server, client_info, PacketType::ClientLoginRequest,
                  "Username and password must not be empty!");
        return;
    }

    auto user_login = UserManager::getInstance().findByUsernameAndPassword(
        username_login, password_login);

    if (user_login.has_value()) {
        int user_id = user_login.value().get_user_id();
        auto& client = ClientManager::getInstance()[client_info.ID];
        client = Authenticator::getInstance().add_session(user_id);

        SendClientConnectionSuccess(m_server, client_info, PacketType::ClientLoginRequest,
                                    client);
    } else {
        SendError(m_server, client_info, PacketType::ClientLoginRequest,
                  "Login credentials incorrect!");
    }
}


void ClientLogoutHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                     Walnut::BufferStreamReader& stream) {}

void SendError(std::shared_ptr<Walnut::Server> server, const Walnut::ClientInfo& client,
                            PacketType packet_type,
                            std::string_view error_msg) {

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);
    stream.WriteRaw< PacketType >(packet_type);
    stream.WriteRaw< bool >(false);
    stream.WriteString(error_msg);

    server->SendBufferToClient(
        client.ID,
        Walnut::Buffer(scratch_buffer, stream.GetStreamPosition()));
}

void SendClientConnectionSuccess(std::shared_ptr< Walnut::Server > server,
                                 const
                                     Walnut::ClientInfo& client,
                                              PacketType packet_type,
                                              Session session) {
    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);
    
    Walnut::BufferStreamWriter stream(scratch_buffer);
    stream.WriteRaw< PacketType >(packet_type);
    stream.WriteRaw< bool >(true);
    stream.WriteObject(session);

    server->SendBufferToClient(
        client.ID,
        Walnut::Buffer(scratch_buffer, stream.GetStreamPosition()));

    //console.AddItalicMessage("Info", "Client {} connected",
    //                           client.ConnectionDesc);
}