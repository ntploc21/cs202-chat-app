#pragma once

#include "ServerController.hpp"
#include "Walnut/Layer.h"
#include "Walnut/Networking/Server.h"
#include "Walnut/UI/Console.h"
#include "ServerPacket.hpp"
#include "Session.hpp"

#include "Walnut/Image.h"

class ServerLayer : public Walnut::Layer {
public:
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUIRender() override;
    virtual void OnUpdate(float ts) override;

private:
    /* Server event callbacks */
    void OnClientConnected(const Walnut::ClientInfo& client);
    void OnClientDisconnected(const Walnut::ClientInfo& client);
    void OnDataReceived(const Walnut::ClientInfo& client_info,
                        const Walnut::Buffer buffer);

    // void SaveMessageHistoryToFile(const std::filesystem::path& filepath);
    // bool LoadMessageHistoryFromFile(const std::filesystem::path& filepath);
    
    /* Utils */
    /*void SendError(const Walnut::ClientInfo& client, PacketType packet_type, std::string_view error_msg);
    void SendClientConnectionSuccess(const Walnut::ClientInfo& client, PacketType packet_type, Session session);*/
private:
    std::shared_ptr< Walnut::Server > m_server{};

    Walnut::UI::Console m_console{"Server Console"};

    Walnut::Buffer m_scratch_buffer{};

    ServerController m_server_controller{};
    std::map< Walnut::ClientID, Session > m_connected_clients{};

    

    std::shared_ptr< Walnut::Image > m_test_avt;

};