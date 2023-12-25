#pragma once

#include "User.hpp"

#include "Walnut/Layer.h"
#include "Walnut/Networking/Client.h"
#include "Walnut/UI/Console.h"

class ClientLayer : public Walnut::Layer {
public:
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUIRender() override;

    bool IsConnected() const;
    void OnDisconnectButton();

private:
    // UI
    void UI_ConnectionModal();
    void UI_Login();
    void UI_Register();

    // Server event callbacks
    void OnConnected();
    void OnDisconnected();
    void OnDataReceived(const Walnut::Buffer buffer);

private:
    std::unique_ptr< Walnut::Client > m_client{};
    std::string m_server_ip{};

    Walnut::Buffer m_scratch_buffer{};

    User current_user{};

    bool m_connection_modal_open = false;
    bool m_login_modal_open = false;
    bool m_login = false;
    bool m_remember_me = false;
};