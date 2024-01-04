#pragma once

#include "User.hpp"
#include "Walnut/Image.h"
#include "Walnut/Layer.h"
#include "Walnut/Networking/Client.h"
#include "Walnut/UI/Console.h"
#include "date/date.h"

#include "DirectMessage.hpp"
#include "GroupMessage.hpp"

#include "ServerPacket.hpp"

class ClientLayer : public Walnut::Layer {
private:
    enum class Tab { Default, Chat, Contact, Settings };

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
    void UI_MainCenterDM();
    void UI_MainCenterGroup();


    void UI_Info();
    void UI_UserInfo();

    // Friends UI
    void UI_Friends();
    void UI_FriendList();
    void UI_Groups();
    void UI_FriendsRequest();
    void UI_FriendItem(const User& user, std::string& note);

    // Server event callbacks
    void OnConnected();
    void OnDisconnected();
    void OnDataReceived(const Walnut::Buffer buffer);

    // Save + Load credentials
    void SaveCredentials();
    void LoadCredentials();
    void DeleteCredentials();

    void ConnectToServer();
    void LoginToServer();

    // Send no argument packet
    void SendPacket(PacketType packet_type);

private:
    std::unique_ptr< Walnut::Client > m_client{};
    std::string m_server_ip{};

    Walnut::Buffer m_scratch_buffer{};

    User m_current_user{};

    /* */
    bool m_connection_modal_open = false;
    bool m_login_modal_open = false;
    bool m_login = false;
    bool m_logged_in = false;
    bool m_remember_me = false;

    Tab m_current_tab = Tab::Default;

    bool m_AutoScroll = true;
    bool m_ScrollToBottom = false;

    bool m_friends_filter = false;

    bool m_view_profile = false;

    bool m_has_credentials = false;

    /* */

    using MessageSendCallback = std::function< void(std::string_view) >;

    std::string m_MessageBuffer;
    MessageSendCallback m_MessageSendCallback;

    /* */

    std::shared_ptr< Walnut::Image > m_test_avt;

    const std::string m_saved_credentials_file{"credentials.yaml"};

    std::vector< User > m_friends{};
    std::vector< User > m_pending_friends{};

    std::vector< std::string > m_pending_friend_notes{};
    std::map< int, std::string > m_user_notes{};

    User m_profile_user{};

private:
    struct Chat {
        int type;  // 0: DM, 1: Group
        int id;
        date::sys_seconds m_last_message_at{};

        /*bool operator<(const Chat& other) const {
            return m_last_message_at > other.m_last_message_at;
        }*/
    };

    Chat m_chat{};
};