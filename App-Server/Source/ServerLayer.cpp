#include "ServerLayer.hpp"

#include "Authenticator.hpp"
#include "ServerPacket.hpp"
#include "UserManager.hpp"
#include "Walnut/Core/Assert.h"
#include "Walnut/Serialization/BufferStream.h"

void ServerLayer::OnAttach() {
    const int port = 8192;

    m_scratch_buffer.Allocate(8192);

    m_server = std::make_shared< Walnut::Server >(port);
    m_server->SetClientConnectedCallback(
        [this](const Walnut::ClientInfo& clientInfo) {
            OnClientConnected(clientInfo);
        });

    m_server->SetClientDisconnectedCallback(
        [this](const Walnut::ClientInfo& clientInfo) {
            OnClientDisconnected(clientInfo);
        });

    m_server->SetDataReceivedCallback(
        [this](const Walnut::ClientInfo& clientInfo,
               const Walnut::Buffer data) {
            OnDataReceived(clientInfo, data);
        });
    m_server->Start();

    m_server_controller = ServerController(m_server);

    m_console.AddTaggedMessage("Info", "Started server on port {}", port);
}

void ServerLayer::OnDetach() {
    m_server->Stop();

    m_scratch_buffer.Release();
}

void ServerLayer::OnUpdate(float ts) {
    // ...
}

void ServerLayer::OnUIRender() {
    {
        /*ImGui::Begin("Client Info");
        ImGui::Text("Connected clients: %d", m_ConnectedClients.size());

        static bool selected = false;
        for (const auto& [id, name] : m_ConnectedClients) {
            if (name.Username.empty()) continue;

            ImGui::Selectable(name.Username.c_str(), &selected);
            if (ImGui::IsItemHovered()) {
                // Get some more info about client from server
                const auto& clientInfo = m_Server->GetConnectedClients().at(id);

                ImGui::BeginTooltip();
                ImGui::SetTooltip(clientInfo.ConnectionDesc.c_str());
                ImGui::EndTooltip();
            }
        }
        ImGui::End();*/
    }

    m_console.OnUIRender();
}

void ServerLayer::OnClientConnected(const Walnut::ClientInfo& client) {
    // Client connection is handled in the PacketType::ClientConnectionRequest
    // packet
}

void ServerLayer::OnClientDisconnected(const Walnut::ClientInfo& client) {
    // ...
}

void ServerLayer::OnDataReceived(const Walnut::ClientInfo& client_info,
                                 const Walnut::Buffer buffer) {
    m_server_controller.handlePacket(client_info, buffer);
    /*Walnut::BufferStreamReader stream(buffer);

    PacketType type;
    bool success = stream.ReadRaw< PacketType >(type);
    WL_CORE_VERIFY(success);
    std::cout << "Packet type: " << PacketTypeToString(type) << std::endl;
    if (!success)  // Why couldn't we read packet type? Probs invalid packet
        return;

    switch (type) {
        case PacketType::ClientRegisterRequest: {
            std::string username;
            std::string password;
            std::string fullname;

            if (!stream.ReadString(username) || !stream.ReadString(password) ||
                !stream.ReadString(fullname)) {
                SendError(client_info, PacketType::ClientRegisterRequest,
                          "Username, password and fullname must not be empty!");
                return;
            }

            auto user = UserManager::getInstance().findByUsernameAndPassword(
                username, password);

            if (user.has_value()) {
                SendError(client_info, PacketType::ClientRegisterRequest,
                          "User already exists");
            } else {
                User new_user(username, password, fullname);
                int user_id = UserManager::getInstance().add_user(new_user);

                auto& client = m_connected_clients[client_info.ID];
                client = Authenticator::getInstance().add_session(user_id);

                SendClientConnectionSuccess(
                    client_info, PacketType::ClientRegisterRequest, client);
            }

            break;
        }
        case PacketType::ClientLoginRequest: {
            std::string username_login;
            std::string password_login;

            if (!stream.ReadString(username_login) ||
                !stream.ReadString(password_login)) {
                SendError(client_info, PacketType::ClientLoginRequest,
                          "Username and password must not be empty!");
                return;
            }

            auto user_login =
                UserManager::getInstance().findByUsernameAndPassword(
                    username_login, password_login);

            if (user_login.has_value()) {
                int user_id = user_login.value().get_user_id();
                auto& client = m_connected_clients[client_info.ID];
                client = Authenticator::getInstance().add_session(user_id);

                SendClientConnectionSuccess(
                    client_info, PacketType::ClientLoginRequest, client);
            } else {
                SendError(client_info, PacketType::ClientLoginRequest,
                          "Login credentials incorrect!");
            }

            break;
        }
    }*/
}

/*void ServerLayer::SendError(const Walnut::ClientInfo& client,
                            PacketType packet_type,
                            std::string_view error_msg) {
    Walnut::BufferStreamWriter stream(m_scratch_buffer);
    stream.WriteRaw< PacketType >(packet_type);
    stream.WriteRaw< bool >(false);
    stream.WriteString(error_msg);

    m_server->SendBufferToClient(
        client.ID,
        Walnut::Buffer(m_scratch_buffer, stream.GetStreamPosition()));
}

void ServerLayer::SendClientConnectionSuccess(const Walnut::ClientInfo& client,
                                              PacketType packet_type,
                                              Session session) {
    Walnut::BufferStreamWriter stream(m_scratch_buffer);
    stream.WriteRaw< PacketType >(packet_type);
    stream.WriteRaw< bool >(true);
    stream.WriteObject(session);

    m_server->SendBufferToClient(
        client.ID,
        Walnut::Buffer(m_scratch_buffer, stream.GetStreamPosition()));

    m_console.AddItalicMessage("Info", "Client {} connected",
                               client.ConnectionDesc);
}*/
