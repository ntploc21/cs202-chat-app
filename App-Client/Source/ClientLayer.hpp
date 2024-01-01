#pragma once

#include "User.hpp"

#include "Walnut/Layer.h"
#include "Walnut/Networking/Client.h"
#include "Walnut/UI/Console.h"

#include "Walnut/Image.h"

class ClientLayer : public Walnut::Layer {
private:
    enum class Tab {
        Default,
        Chat,
        Contact,
        Settings
    };

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

    // Chat UI
    void UI_MainChat();
    void UI_MainTab();
    void UI_UserList();
    void UI_MainCenter();
    void UI_Info();

    // Friends UI
    void UI_Friends();
    void UI_FriendList();
    void UI_Groups();
    void UI_FriendsRequest();

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
    bool m_logged_in = false;
    bool m_remember_me = false;
    
    Tab m_current_tab = Tab::Chat;
    
    bool m_AutoScroll = true;
    bool m_ScrollToBottom = false;

    bool m_friends_filter = false;

    using MessageSendCallback = std::function< void(std::string_view) >;

    
	std::string m_MessageBuffer;
    MessageSendCallback m_MessageSendCallback;

    std::shared_ptr< Walnut::Image > m_test_avt;
};