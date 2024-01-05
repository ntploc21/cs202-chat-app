#include "ClientLayer.hpp"

#include <imgui.h>

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "ConversationManager.hpp"
#include "DirectMessageManager.hpp"
#include "GroupMessageManager.hpp"
#include "IconsFontAwesome5.h"
#include "MessageManager.hpp"
#include "ServerPacket.hpp"
#include "Session.hpp"
#include "UserManager.hpp"
#include "Walnut/Application.h"
#include "Walnut/ApplicationGUI.h"
#include "Walnut/Networking/NetworkingUtils.h"
#include "Walnut/Serialization/BufferStream.h"
#include "Walnut/UI/UI.h"
#include "Walnut/Utils/StringUtils.h"
#include "date/tz.h"
#include "misc/cpp/imgui_stdlib.h"
#include "yaml-cpp/yaml.h"

#include "DirectMessageFilter.hpp"

void ClientLayer::OnAttach() {
    m_scratch_buffer.Allocate(8192);

    m_client = std::make_unique< Walnut::Client >();
    m_client->SetServerConnectedCallback([this]() { OnConnected(); });
    m_client->SetServerDisconnectedCallback([this]() { OnDisconnected(); });
    m_client->SetDataReceivedCallback(
        [this](const Walnut::Buffer data) { OnDataReceived(data); });

    m_MessageSendCallback = [this](std::string_view message) {
        SendChatMessage(message);
    };

    // ImGui::StyleColorsLight();

    m_test_avt = std::make_shared< Walnut::Image >("Image/test.png");

    UserManager::getInstance().set_used_by_client();
    MessageManager::getInstance().set_used_by_client();
    ConversationManager::getInstance().set_used_by_client();
    DirectMessageManager::getInstance().set_used_by_client();
    GroupMessageManager::getInstance().set_used_by_client();

    LoadCredentials();
}

void ClientLayer::OnDetach() {
    m_client->Disconnect();

    m_scratch_buffer.Release();
}

void ClientLayer::OnUIRender() {
    if (m_current_tab == Tab::Default) {
        UI_ConnectionModal();
        UI_Login();
        UI_Register();
    }

    if (!m_logged_in || !IsConnected()) {
        m_current_tab = Tab::Default;
        return;
    }

    if (m_current_tab == Tab::Chat) {
        UI_MainChat();
    }

    if (m_current_tab == Tab::Contact) {
        UI_Friends();
    }

    if (m_current_tab == Tab::Settings) {
        UI_Settings();
    }

    if (m_view_profile) {
        ImGui::OpenPopup("Profile");
        m_view_profile = false;
    }

    if (ImGui::BeginPopupModal("Profile", nullptr, ImGuiWindowFlags_MenuBar)) {
        UI_UserInfo();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));

        if (ImGui::Button(ICON_FA_SIGN_OUT_ALT "Close"))
            ImGui::CloseCurrentPopup();

        ImGui::PopStyleVar(2);

        ImGui::EndPopup();
    }

    if (m_change_nickname_modal) {
        ImGui::OpenPopup("Change nickname");
        m_change_nickname_modal = false;
    }

    if (ImGui::BeginPopupModal("Change nickname", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("New nickname");
        ImGui::InputText("##new_nickname", &m_change_nickname);

        if (ImGui::Button("Change")) {
            UpdateNickname(m_change_nickname_user_id, m_change_nickname);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

bool ClientLayer::IsConnected() const {
    return m_client->GetConnectionStatus() ==
           Walnut::Client::ConnectionStatus::Connected;
}

void ClientLayer::OnDisconnectButton() { m_client->Disconnect(); }

void ClientLayer::UI_ConnectionModal() {
    if (m_client->GetConnectionStatus() ==
        Walnut::Client::ConnectionStatus::Connected) {
        if (m_has_credentials) {
            LoginToServer();
            m_has_credentials = false;
        }

        return;
    }

    if (!m_connection_modal_open) {
        ImGui::OpenPopup("Connect to server");
    }

    m_connection_modal_open = ImGui::BeginPopupModal(
        "Connect to server", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (m_connection_modal_open) {
        ImGui::Text("Server Address");
        ImGui::InputText("##address", &m_server_ip);
        ImGui::SameLine();
        if (ImGui::Button("Connect")) {
            ConnectToServer();
        }

        if (Walnut::UI::ButtonCentered("Quit"))
            Walnut::Application::Get().Close();

        if (m_client->GetConnectionStatus() ==
            Walnut::Client::ConnectionStatus::Connected) {
            // Wait for response
            ImGui::CloseCurrentPopup();

        } else if (m_client->GetConnectionStatus() ==
                   Walnut::Client::ConnectionStatus::FailedToConnect) {
            ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.1f, 1.0f),
                               "Connection failed.");
            const auto& debugMessage = m_client->GetConnectionDebugMessage();
            if (!debugMessage.empty())
                ImGui::TextColored(ImVec4(0.9f, 0.2f, 0.1f, 1.0f),
                                   debugMessage.c_str());
        } else if (m_client->GetConnectionStatus() ==
                   Walnut::Client::ConnectionStatus::Connecting) {
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Connecting...");
        }

        ImGui::EndPopup();
    }
}

void ClientLayer::UI_Login() {
    if (m_client->GetConnectionStatus() !=
            Walnut::Client::ConnectionStatus::Connected ||
        !m_login || m_logged_in)
        return;

    std::string popup_title = "Connected to server at " + m_server_ip;

    if (!m_login_modal_open) {
        ImGui::OpenPopup(popup_title.c_str());
    }

    m_login_modal_open = ImGui::BeginPopupModal(
        popup_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (m_login_modal_open) {
        if (ImGui::CollapsingHeader("Login to an existing account",
                                    ImGuiTreeNodeFlags_None)) {
        }
        ImGui::Text("Username");
        ImGui::InputText("##username", &m_current_user.m_username);

        ImGui::Text("Password");
        ImGui::InputText("##password", &m_current_user.m_password,
                         ImGuiInputTextFlags_Password);

        ImGui::Checkbox("Remember me", &m_remember_me);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Not have an account yet?");
        ImGui::SameLine();
        if (ImGui::Button("Sign up")) {
            m_login = false;

            ImGui::CloseCurrentPopup();
        }

        ImGuiStyle& style = ImGui::GetStyle();

        float actualSize = ImGui::CalcTextSize("Sign in").x +
                           ImGui::CalcTextSize("Quit").x +
                           style.FramePadding.x * 4.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - actualSize) * 0.5f;
        if (off > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

        if (ImGui::Button("Sign in")) LoginToServer();

        ImGui::SameLine();

        if (ImGui::Button("Back")) {
            OnDisconnectButton();
            m_login = false;

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ClientLayer::UI_Register() {
    if (m_client->GetConnectionStatus() !=
            Walnut::Client::ConnectionStatus::Connected ||
        m_login || m_logged_in)
        return;

    std::string popup_title = "Connected to server at " + m_server_ip;

    if (!m_login_modal_open) {
        ImGui::OpenPopup(popup_title.c_str());
    }

    m_login_modal_open = ImGui::BeginPopupModal(
        popup_title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (m_login_modal_open) {
        if (ImGui::CollapsingHeader("Register a new account",
                                    ImGuiTreeNodeFlags_None)) {
        }
        ImGui::Text("Fullname");
        ImGui::InputText("##fullname", &m_current_user.m_fullname);

        ImGui::Text("Username");
        ImGui::InputText("##username", &m_current_user.m_username);

        ImGui::Text("Password");
        ImGui::InputText("##password", &m_current_user.m_password,
                         ImGuiInputTextFlags_Password);

        ImGui::Checkbox("Remember me", &m_remember_me);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Already have an account?");
        ImGui::SameLine();
        if (ImGui::Button("Sign in")) {
            m_login = true;

            ImGui::CloseCurrentPopup();
        }

        ImGuiStyle& style = ImGui::GetStyle();

        float actualSize = ImGui::CalcTextSize("Sign up").x +
                           ImGui::CalcTextSize("Back").x +
                           style.FramePadding.x * 4.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - actualSize) * 0.5f;
        if (off > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

        if (ImGui::Button("Sign up")) {
            Walnut::BufferStreamWriter stream(m_scratch_buffer);
            stream.WriteRaw< PacketType >(PacketType::ClientRegisterRequest);
            stream.WriteString(m_current_user.m_username);
            stream.WriteString(m_current_user.m_password);
            stream.WriteString(m_current_user.m_fullname);

            m_client->SendBuffer(stream.GetBuffer());

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Back")) {
            OnDisconnectButton();

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ClientLayer::UI_MainChat() {
    /*if (m_client->GetConnectionStatus() !=
            Walnut::Client::ConnectionStatus::Connected ||
        !m_logged_in)
        return;*/

    UI_MainTab();
    UI_UserList();
    UI_MainCenter();
    UI_Info();
}

void ClientLayer::UI_MainTab() {
    ImGui::SetNextWindowSize(ImVec2(70, 800), ImGuiCond_FirstUseEver);

    // begin but hide the title
    bool open = true;
    ImGui::Begin("Main Tab", &open,
                 ImGuiWindowFlags_NoDecoration | ImGuiDockNodeFlags_NoTabBar);

    // draw rounded avatar image
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
    ImGui::PopStyleVar(2);

    ImGui::SameLine();

    // edit the opacity of the avatar selectable

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 56);

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    if (ImGui::Selectable("##avatar", false, ImGuiSelectableFlags_None,
                          ImVec2(50, 50))) {
        ImGui::OpenPopup("##Info");
    }

    ImGui::PopStyleVar(2);

    ImGui::PopStyleColor(3);

    if (ImGui::BeginPopup("##Info")) {
        ImGui::PushFont(Walnut::Application::GetFont("Bold"));

        ImGui::Text(m_current_user.get_fullname().c_str());

        ImGui::PopFont();

        ImGui::Separator();

        if (ImGui::Selectable("Profile", false, ImGuiSelectableFlags_None,
                              ImVec2(200, 0))) {
            // open profile modal
            m_profile_user = m_current_user;
            m_view_profile = true;
        }

        if (ImGui::Selectable("Logout", false, ImGuiSelectableFlags_None,
                              ImVec2(200, 0))) {
            Walnut::BufferStreamWriter stream(m_scratch_buffer);
            stream.WriteRaw< PacketType >(PacketType::ClientLogoutRequest);

            m_client->SendBuffer(stream.GetBuffer());
        }

        ImGui::EndPopup();
    }

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesomeBig"));

    // center the icon in the selectable
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.7f));
    // margin 0
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    // center the icon vertically in the selectable

    if (ImGui::Selectable(ICON_FA_COMMENT, (m_current_tab == Tab::Chat),
                          ImGuiSelectableFlags_None, ImVec2(50, 50))) {
        m_current_tab = Tab::Chat;
    }

    if (ImGui::Selectable(ICON_FA_ADDRESS_BOOK, (m_current_tab == Tab::Contact),
                          ImGuiSelectableFlags_None, ImVec2(50, 50))) {
        m_current_tab = Tab::Contact;
    }

    if (ImGui::Selectable(ICON_FA_COG, (m_current_tab == Tab::Settings),
                          ImGuiSelectableFlags_None, ImVec2(50, 50))) {
        m_current_tab = Tab::Settings;
    }

    ImGui::PopStyleVar(3);

    ImGui::PopFont();

    ImGui::End();
}

void ClientLayer::UI_UserList() {
    ImGui::SetNextWindowSize(ImVec2(350, 800), ImGuiCond_FirstUseEver);

    bool open = true;
    ImGui::Begin("List", &open,
                 ImGuiWindowFlags_NoDecoration | ImGuiDockNodeFlags_NoTabBar);

    static std::string search;

    ImGui::Text("Search");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(180);
    ImGui::InputText("##Search", &search);
    ImGui::SameLine();

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

    // ImGui::Button(ICON_FA_SEARCH " Search");

    ImGui::Button(ICON_FA_USER_PLUS);
    ImGui::SameLine();
    ImGui::Button(ICON_FA_USERS);

    ImGui::PopFont();

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("##Tabs", tab_bar_flags)) {
        if (ImGui::BeginTabItem("Inbox")) {
            // design an inbox that looks similar to messenger inbox

            std::vector< DirectMessage > direct_messages =
                DirectMessageManager::getInstance().get_direct_messages();

            if(!search.empty()) {
                auto dm_spec =
                    DMNameSpecification(m_current_user.get_user_id(), search) ||
                    DMLastMessageSpecification(search);

                DirectMessageFilter dm_filter;

                direct_messages = dm_filter.filter(direct_messages, dm_spec);
            }

            // sort all direct messages by last message at

            std::sort(
                direct_messages.begin(), direct_messages.end(),
                [](const DirectMessage& a, const DirectMessage& b) {
                    auto a_last_message =
                        ConversationManager::getInstance().get_last_message(
                            a.get_conversation_id());
                    auto b_last_message =
                        ConversationManager::getInstance().get_last_message(
                            b.get_conversation_id());
                    if (!a_last_message.has_value()) return false;
                    if (!b_last_message.has_value()) return true;

                    return a_last_message.value().m_created_at >
                           b_last_message.value().m_created_at;
                });

            for (auto& direct_message : direct_messages) {
                if (direct_message.get_user_id_2() ==
                    m_current_user.get_user_id()) {
                    std::swap(direct_message.m_user_id_1,
                              direct_message.m_user_id_2);
                    std::swap(direct_message.m_user_1_nickname,
                              direct_message.m_user_2_nickname);
                }

                ImGui::BeginGroup();

                ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
                ImGui::SameLine();

                ImGui::BeginGroup();

                ImGui::PushFont(Walnut::Application::GetFont("Bold"));

                std::string_view name{direct_message.m_user_2_nickname};

                ImGui::Text(name.data());

                ImGui::PopFont();

                auto message =
                    ConversationManager::getInstance().get_last_message(
                        direct_message.get_conversation_id());

                if (message.has_value()) {
                    std::string content = message.value().m_content;

                    if (message.value().get_from_id() ==
                        m_current_user.get_user_id()) {
                        content = "You: " + content;
                    }

                    ImGui::Text(message.value().m_content.c_str());
                    ImGui::SameLine();

                    // if the current user's last seen at < the last message's
                    // created at, bold the text

                    date::sys_seconds last_seen =
                        direct_message.m_user_1_last_seen_at;

                    if (last_seen < message.value().m_created_at) {
                        ImGui::PushFont(Walnut::Application::GetFont("Bold"));
                    }

                    ImGui::PushFont(Walnut::Application::GetFont("Italic"));

                    date::sys_seconds last_message_at =
                        message.value().m_created_at;
                    last_message_at += std::chrono::hours(7);

                    ImGui::Text(
                        date::format("(%H:%M %p)", last_message_at).c_str());

                    ImGui::PopFont();

                    if (last_seen < message.value().m_created_at)
                        ImGui::PopFont();

                } else {
                    ImGui::Text("No message");
                }

                ImGui::EndGroup();

                ImGui::EndGroup();

                // if hover, change the mouse cursor to hand
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                }

                // if clicked, change the chat to this chat
                if (ImGui::IsItemClicked()) {
                    ToDirectMessage(direct_message.get_dm_id());
                }

                ImGui::Separator();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Group")) {
            /*std::vector< GroupMessage > group_messages =
                GroupMessageManager::getInstance().get_group_messages();

            // sort all direct messages by last message at

            std::sort(
                group_messages.begin(), group_messages.end(),
                [](const DirectMessage& a, const DirectMessage& b) {
                    auto a_last_message =
                        ConversationManager::getInstance().get_last_message(
                            a.get_conversation_id());
                    auto b_last_message =
                        ConversationManager::getInstance().get_last_message(
                            b.get_conversation_id());
                    if (!a_last_message.has_value()) return false;
                    if (!b_last_message.has_value()) return true;

                    return a_last_message.value().m_created_at >
                           b_last_message.value().m_created_at;
                });

            for (auto& group_message : group_messages) {

                ImGui::BeginGroup();

                ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
                ImGui::SameLine();

                ImGui::BeginGroup();

                ImGui::PushFont(Walnut::Application::GetFont("Bold"));

                std::string_view name{group_message.get_group_name()};

                ImGui::Text(name.data());

                ImGui::PopFont();

                auto message =
                    ConversationManager::getInstance().get_last_message(
                        group_message.get_conversation_id());

                if (message.has_value()) {
                    std::string content = message.value().m_content;

                    if (message.value().get_from_id() ==
                        m_current_user.get_user_id()) {
                        content = "You: " + content;
                    }

                    ImGui::Text(message.value().m_content.c_str());
                    ImGui::SameLine();

                    ImGui::PushFont(Walnut::Application::GetFont("Italic"));

                    date::sys_seconds last_message_at =
                        message.value().m_created_at;
                    last_message_at += std::chrono::hours(7);

                    ImGui::Text(
                        date::format("(%H:%M %p)", last_message_at).c_str());

                    ImGui::PopFont();

                } else {
                    ImGui::Text("No message");
                }

                ImGui::EndGroup();

                ImGui::EndGroup();

                // if hover, change the mouse cursor to hand
                if (ImGui::IsItemHovered()) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                }

                // if clicked, change the chat to this chat
                if (ImGui::IsItemClicked()) {
                    ToGroupMessage(group_message.get_group_id());
                }

                ImGui::Separator();
            }*/

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void ClientLayer::UI_MainCenter() {
    ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);

    bool open = true;
    ImGui::Begin("Main Chat", &open,
                 ImGuiWindowFlags_NoDecoration | ImGuiDockNodeFlags_NoTabBar);

    if (m_chat.id) {
        if (!m_chat.type)
            UI_MainCenterDM();
        else
            UI_MainCenterGroup();
    }

    ImGui::End();
}

void ClientLayer::UI_MainCenterDM() {
    DirectMessage direct_message = DirectMessageManager::getInstance()
                                       .get_direct_message(m_chat.id)
                                       .value();

    Conversation conversation =
        ConversationManager::getInstance()
            .get_conversation(direct_message.get_conversation_id())
            .value();
    std::vector< Message > messages = conversation.get_messages();
    std::vector< int > pin_msg_ids = direct_message.get_pin_message_list_ids();

    if (direct_message.get_user_id_2() == m_current_user.get_user_id()) {
        std::swap(direct_message.m_user_id_1, direct_message.m_user_id_2);
        std::swap(direct_message.m_user_1_nickname,
                  direct_message.m_user_2_nickname);
        std::swap(direct_message.m_user_1_last_seen_at,
                  direct_message.m_user_2_last_seen_at);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));

    ImGui::PopStyleVar(2);

    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::PushFont(Walnut::Application::GetFont("Bold"));

    std::string name = direct_message.m_user_2_nickname;

    ImGui::Text(name.c_str());

    ImGui::PopFont();

    // draw colorful center dot to indicate status

    ImGui::SameLine();

    // if online, draw green dot

    if (UserManager::getInstance()
            .get_user(direct_message.m_user_id_2)
            .value()
            .is_online()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

        ImGui::BulletText("");

        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        ImGui::BulletText("");

        ImGui::PopStyleColor();
    }

    date::sys_seconds last_seen_at = direct_message.m_user_2_last_seen_at;
    last_seen_at += std::chrono::hours(7);

    if (date::floor< std::chrono::duration< int > >(
            std::chrono::system_clock::now()) -
            last_seen_at <
        std::chrono::hours(24)) {
        ImGui::SameLine();

        ImGui::PushFont(Walnut::Application::GetFont("Italic"));

        ImGui::Text(date::format("(last seen %H:%M %p)", last_seen_at).c_str());

        ImGui::PopFont();
    }

    ImGui::EndGroup();

    ImGui::Separator();

    const float TextPadding = 8.0f;

    // draw the chat box
    {
        // Reserve enough left-over height for 1 separator + 1 input text
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        const float footer_height_to_reserve =
            ImGui::GetStyle().ItemSpacing.y +
            ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion",
                          ImVec2(0, -footer_height_to_reserve), false,
                          ImGuiWindowFlags_HorizontalScrollbar);

        // let the latest message be at the bottom
        // all messages should start at the bottom of the window

        /*if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }*/

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(4, 1));  // Tighten spacing

        ImGui::SetCursorPosY(TextPadding);

        for (int i = 0, j = 0; i < messages.size(); i = j) {
            while (j < messages.size() &&
                   messages[i].get_from_id() == messages[j].get_from_id())
                j++;

            if (messages[i].get_from_id() == m_current_user.get_user_id()) {
                // message from you (only the message and the time)
                // messages are aligned to the right and in light blue
                // align in such a way that the the padding between the message
                // and the right side of the window is 8px if the message is too
                // long, it should be wrapped to the next line the width of a
                // message should not exceed 1/2 of the window width

                for (int k = i; k < j; k++) {
                    std::string content = messages[k].get_content();
                    float width = ImGui::CalcTextSize(content.c_str()).x;

                    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - width -
                                         TextPadding - 25);

                    // color the background of the text wrapped

                    ImGui::PushStyleColor(ImGuiCol_Button,
                                          ImVec4(0.0f, 0.0f, 1.0f, 0.1f));

                    ImGui::Button(
                        (content + "##dm_msg" + std::to_string(k)).c_str());
                    if (ImGui::BeginPopupContextItem(
                            ("##message_dm_me_" + std::to_string(k)).c_str())) {
                        // add some options: pin, forward, unsend for everyone

                        // check if the message is pinned

                        bool is_pinned = false;

                        for (auto& id : pin_msg_ids) {
                            if (id == messages[k].get_msg_id()) {
                                is_pinned = true;
                                break;
                            }
                        }

                        if (!is_pinned) {
                            if (ImGui::Button("Pin")) {
                                PinMessage(messages[k].get_msg_id());
                            }
                        } else {
                            if (ImGui::Button("Unpin")) {
                                UnpinMessage(messages[k].get_msg_id());
                            }
                        }

                        if (ImGui::Button("Unsend for everyone")) {
                            DeleteMessage(messages[k].get_msg_id());
                        }

                        ImGui::EndPopup();
                    }

                    ImGui::PopStyleColor();
                }

                // if the current time > last_message_at + 24 hours, don't show
                // the time

                date::sys_seconds msg_sent_at = messages[j - 1].m_created_at;
                msg_sent_at += std::chrono::hours(7);

                auto time_str = date::format("%H:%M %p", msg_sent_at);

                if (date::floor< std::chrono::duration< int > >(
                        std::chrono::system_clock::now()) -
                        msg_sent_at <
                    std::chrono::hours(24)) {
                    ImGui::PushFont(Walnut::Application::GetFont("Italic"));

                    ImGui::SetCursorPosX(
                        ImGui::GetWindowSize().x -
                        ImGui::CalcTextSize(time_str.c_str()).x - TextPadding -
                        10);

                    ImGui::Text(time_str.c_str());

                    ImGui::PopFont();
                }
            } else {
                // message from others (avatar, name, message and time)
                // messages are aligned to the left and in white
                // align in such a way that the the padding between the message
                // and the left side of the window is 8px if the message is too
                // long, it should be wrapped to the next line the width of a
                // message should not exceed 1/2 of the window width

                ImGui::BeginGroup();

                ImGui::NewLine();

                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 +
                                     (j - i - 1) * 33);

                ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(28, 28));

                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();

                ImGui::PushFont(Walnut::Application::GetFont("Italic"));

                std::string name = direct_message.m_user_2_nickname;

                ImGui::Text(name.c_str());

                ImGui::PopFont();

                for (int k = i; k < j; k++) {
                    std::string content = messages[k].get_content();
                    float width = ImGui::CalcTextSize(content.c_str()).x;

                    ImGui::Button(content.c_str());
                    if (ImGui::BeginPopupContextItem(
                            ("##message_dm_other_" + std::to_string(k))
                                .c_str())) {
                        // add some options: pin, forward, unsend for everyone

                        // check if the message is pinned

                        bool is_pinned = false;

                        for (auto& id : pin_msg_ids) {
                            if (id == messages[k].get_msg_id()) {
                                is_pinned = true;
                                break;
                            }
                        }

                        if (!is_pinned) {
                            if (ImGui::Selectable("Pin")) {
                                PinMessage(messages[k].get_msg_id());
                            }
                        } else {
                            if (ImGui::Selectable("Unpin")) {
                                UnpinMessage(messages[k].get_msg_id());
                            }
                        }

                        ImGui::EndPopup();
                    }
                }
                date::sys_seconds msg_sent_at = messages[j - 1].m_created_at;
                msg_sent_at += std::chrono::hours(7);

                if (date::floor< std::chrono::duration< int > >(
                        std::chrono::system_clock::now()) -
                        msg_sent_at <
                    std::chrono::hours(24)) {
                    ImGui::PushFont(Walnut::Application::GetFont("Italic"));
                    ImGui::Text(date::format("%H:%M %p", msg_sent_at).c_str());
                    ImGui::PopFont();
                }
                ImGui::EndGroup();
            }
        }
    }

    if (m_ScrollToBottom ||
        (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        ImGui::SetScrollHereY(1.0f);

    m_ScrollToBottom = false;

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleVar();

    // Command-line
    static bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;

    const float sendButtonWidth = 100.0f;
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - sendButtonWidth -
                            TextPadding);
    if (ImGui::InputText("##input", &m_MessageBuffer, input_text_flags)) {
        if (m_MessageSendCallback) m_MessageSendCallback(m_MessageBuffer);

        // Clear InputText
        m_MessageBuffer = "";
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus) {
        ImGui::SetKeyboardFocusHere(-1);  // Auto focus previous widget
        reclaim_focus = false;
    }

    ImGui::SameLine();
    if (ImGui::Button("Send", ImVec2(sendButtonWidth, 0.0f))) {
        if (m_MessageSendCallback) m_MessageSendCallback(m_MessageBuffer);

        // Clear InputText
        m_MessageBuffer = "";
        reclaim_focus = true;
    }
}

void ClientLayer::UI_MainCenterGroup() {}

void ClientLayer::UI_Info() {
    if (!m_chat.id) return;

    DirectMessage direct_message = DirectMessageManager::getInstance()
                                       .get_direct_message(m_chat.id)
                                       .value();

    std::vector< User > members;
    members.push_back(UserManager::getInstance()
                          .get_user(direct_message.m_user_id_1)
                          .value());
    members.push_back(UserManager::getInstance()
                          .get_user(direct_message.m_user_id_2)
                          .value());

    std::vector< std::string > nicknames;
    nicknames.push_back(direct_message.m_user_1_nickname);
    nicknames.push_back(direct_message.m_user_2_nickname);

    std::vector< int > pin_msg_ids = direct_message.get_pin_message_list_ids();

    std::vector< Message > pin_messages;

    for (auto& id : pin_msg_ids) {
        pin_messages.push_back(
            MessageManager::getInstance().get_message(id).value());
    }

    if (direct_message.get_user_id_2() == m_current_user.get_user_id()) {
        std::swap(direct_message.m_user_id_1, direct_message.m_user_id_2);
        std::swap(direct_message.m_user_1_nickname,
                  direct_message.m_user_2_nickname);
        std::swap(direct_message.m_user_1_last_seen_at,
                  direct_message.m_user_2_last_seen_at);
    }

    ImGui::SetNextWindowSize(ImVec2(350, 800), ImGuiCond_FirstUseEver);

    bool open = true;
    ImGui::Begin("Info", &open,
                 ImGuiWindowFlags_NoDecoration | ImGuiDockNodeFlags_NoTabBar);

    // at the center, draw the avatar

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    // center the avatar
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100) * 0.5f);

    ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(100, 100));

    ImGui::PopStyleVar(2);

    // center the name
    std::string name = UserManager::getInstance()
                           .get_user(direct_message.m_user_id_2)
                           .value()
                           .get_fullname();
    ImGui::SetCursorPosX(
        (ImGui::GetWindowSize().x - ImGui::CalcTextSize(name.c_str()).x) *
        0.5f);
    ImGui::PushFont(Walnut::Application::GetFont("Bold"));

    ImGui::Text(name.c_str());

    ImGui::PopFont();

    // center the status

    bool online = UserManager::getInstance()
                      .get_user(direct_message.m_user_id_2)
                      .value()
                      .m_online;

    std::string status = online ? "Active now" : "Offline";
    ImGui::SetCursorPosX(
        (ImGui::GetWindowSize().x - ImGui::CalcTextSize(status.c_str()).x) *
        0.5f);

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), status.c_str());

    // draw three buttons profile, mute and search and it should all on the
    // same line

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));

    // ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);

    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 110) * 0.5f);

    if (ImGui::Button(ICON_FA_USER_CIRCLE)) {
        m_view_profile = true;
        m_profile_user = UserManager::getInstance()
                             .get_user(direct_message.m_user_id_2)
                             .value();
    }
    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_BELL)) {
    }
    ImGui::SameLine();

    if (ImGui::Button(ICON_FA_SEARCH)) {
    }

    ImGui::PopStyleVar(2);

    ImGui::PopFont();

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Members")) {
        // avatar + name + nickname

        for (int i = 0; i < members.size(); i++) {
            ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
            ImGui::SameLine();

            ImGui::BeginGroup();

            ImGui::PushFont(Walnut::Application::GetFont("Bold"));

            ImGui::Text(members[i].get_fullname().c_str());

            ImGui::PopFont();

            ImGui::Text(nicknames[i].c_str());

            ImGui::EndGroup();

            ImGui::SameLine();

            // add a ... button (aligned to right) to show more options

            ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

            ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 50);

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

            std::string label = "Options_dm_##" + std::to_string(i);

            if (ImGui::Button((ICON_FA_ELLIPSIS_V "##" + std::to_string(i) ).c_str())) {
                ImGui::OpenPopup(label.c_str());
            }

            // add a popup with options: view profile, to DM, change nickname
            if (ImGui::BeginPopup(label.c_str())) {
                if (ImGui::Selectable("View profile")) {
                    m_view_profile = true;
                    m_profile_user = members[i];
                }

                if (ImGui::Selectable("To DM")) {
                    ToDirectMessage(direct_message.get_dm_id());
                }

                if (ImGui::Selectable("Change nickname")) {
                    // open a modal to change nickname
                    m_change_nickname_modal = true;
                    m_change_nickname_user_id = members[i].get_user_id();
                    m_change_nickname = nicknames[i];
                }

                ImGui::EndPopup();
            }

            ImGui::PopFont();

            ImGui::Separator();
        }
    }

    // Pin messages
    if (ImGui::CollapsingHeader("Pin Messages")) {
        // avatar + name + message + time

        for (int i = 0; i < pin_messages.size(); i++) {
            ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
            ImGui::SameLine();

            ImGui::BeginGroup();

            ImGui::PushFont(Walnut::Application::GetFont("Bold"));

            ImGui::Text(members[i].get_fullname().c_str());

            ImGui::PopFont();

            ImGui::Text(pin_messages[i].get_content().c_str());

            ImGui::SameLine();

            ImGui::PushFont(Walnut::Application::GetFont("Italic"));

            // display the time in the format (HH:MM AM/PM, DD/MM/YYYY)

            ImGui::Text(date::format("(%H:%M %p, %d/%m/%Y)",
                                     pin_messages[i].m_created_at)
                            .c_str());

            ImGui::PopFont();

            ImGui::EndGroup();

            ImGui::Separator();
        }
    }

    // Shared media (images, files)
    /*if (ImGui::CollapsingHeader("Media and files")) {
        // Media

        // icon media + Title "Media"

        // open modal on click

        // modal contains a list of images

        ImGui::PushFont(Walnut::Application::GetFont("Bold"));
        ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

        // use selectable

        ImGui::Selectable(ICON_FA_IMAGE " Image", false,
                          ImGuiSelectableFlags_None);

        ImGui::Selectable(ICON_FA_FILE " File", false,
                          ImGuiSelectableFlags_None);

        // Files

        ImGui::PopFont();
        ImGui::PopFont();
    }*/

    // Options (Mute, Report, Leave, etc)
    if (ImGui::CollapsingHeader("Options")) {
        ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

        ImGui::Selectable(ICON_FA_BELL " Mute", false,
                          ImGuiSelectableFlags_None);

        ImGui::Selectable(ICON_FA_FLAG " Report", false,
                          ImGuiSelectableFlags_None);

        ImGui::Selectable(ICON_FA_SIGN_OUT_ALT " Leave", false,
                          ImGuiSelectableFlags_None);

        ImGui::PopFont();
    }

    ImGui::End();
}

void ClientLayer::UI_UserInfo() {
    bool self = (m_profile_user.get_user_id() == m_current_user.get_user_id());
    if (self) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        // center the avatar
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100) * 0.5f);

        ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(100, 100));

        ImGui::PopStyleVar(2);

        // center the name
        std::string name = m_profile_user.get_fullname();
        ImGui::SetCursorPosX(
            (ImGui::GetWindowSize().x - ImGui::CalcTextSize(name.c_str()).x) *
            0.5f);
        ImGui::PushFont(Walnut::Application::GetFont("Bold"));

        ImGui::Text(name.c_str());

        ImGui::PopFont();

        ImGui::Separator();

        ImGui::PushFont(Walnut::Application::GetFont("Bold"));

        ImGui::Text("Info");

        ImGui::PopFont();

        ImGui::Text(
            (name + "(@" + m_profile_user.get_username() + ")").c_str());

        int no_of_friends = m_profile_user.m_friend_list.size();
        ImGui::Text(
            ("Number of friends: " + std::to_string(no_of_friends)).c_str());

        ImGui::Separator();

        // draw edit button that spans the whole width

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 160) * 0.5f);

        ImGui::Button(ICON_FA_EDIT " Edit");

        ImGui::SameLine();

        ImGui::PopStyleVar(2);
    } else {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        // center the avatar
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 100) * 0.5f);

        ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(100, 100));

        ImGui::PopStyleVar(2);

        // center the name
        std::string name = m_profile_user.get_fullname();
        ImGui::SetCursorPosX(
            (ImGui::GetWindowSize().x - ImGui::CalcTextSize(name.c_str()).x) *
            0.5f);
        ImGui::PushFont(Walnut::Application::GetFont("Bold"));

        ImGui::Text(name.c_str());

        ImGui::PopFont();

        ImGui::Separator();

        ImGui::PushFont(Walnut::Application::GetFont("Bold"));

        ImGui::Text("Info");

        ImGui::PopFont();

        ImGui::Text(
            (name + "(@" + m_profile_user.get_username() + ")").c_str());

        int no_of_mutual_friends = 0;
        for (auto user : m_friends) {
            for (auto user2_id : m_profile_user.m_friend_list) {
                if (user.get_user_id() == user2_id) {
                    no_of_mutual_friends++;
                    break;
                }
            }
        }

        ImGui::Text(("Mutual friends: " + std::to_string(no_of_mutual_friends))
                        .c_str());

        ImGui::Separator();

        // draw edit button that spans the whole width

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 190) * 0.5f);

        ImGui::Button(ICON_FA_COMMENT " Message");

        ImGui::SameLine();

        ImGui::PopStyleVar(2);
    }
}

void ClientLayer::UI_Friends() {
    UI_MainTab();

    ImGui::SetNextWindowSize(ImVec2(350, 800), ImGuiCond_FirstUseEver);

    bool open = true;
    ImGui::Begin("Friends Tab", &open,
                 ImGuiWindowFlags_NoDecoration | ImGuiDockNodeFlags_NoTabBar);

    // font awesome

    ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None);

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
    if (ImGui::BeginTabItem(ICON_FA_USER " Friends")) {
        UI_FriendList();

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(ICON_FA_USERS " Groups")) {
        UI_Groups();

        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem(ICON_FA_USER_PLUS " Friends Request")) {
        UI_FriendsRequest();

        ImGui::EndTabItem();
    }

    ImGui::PopFont();

    ImGui::EndTabBar();

    ImGui::End();
}

void ClientLayer::UI_FriendList() {
    ImGui::PopFont();

    ImGui::Separator();

    static std::string search;

    ImGui::Text("Search");

    ImGui::SameLine();

    ImGui::SetNextItemWidth(180);

    ImGui::InputText("##Search", &search);

    ImGui::SameLine();

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
    ImGui::PushFont(Walnut::Application::GetFont("Default"));

    static int selected_sort = 0;

    const char* sort_options[] = {ICON_FA_ARROW_UP " Name (A-Z)",
                                  ICON_FA_ARROW_DOWN " Name (Z-A)"};

    if (ImGui::Button(sort_options[selected_sort])) {
        ImGui::OpenPopup("Sort");
    }

    if (ImGui::BeginPopup("Sort")) {
        for (int i = 0; i < IM_ARRAYSIZE(sort_options); i++) {
            if (ImGui::Selectable(sort_options[i])) {
                selected_sort = i;
            }
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    static int selected_filter = 0;

    const char* filter_options[] = {ICON_FA_FILTER " All",
                                    ICON_FA_FILTER " Online"};

    if (ImGui::Button((selected_filter == -1
                           ? "None"
                           : filter_options[selected_filter]))) {
        ImGui::OpenPopup("Filter");
    }

    if (ImGui::BeginPopup("Filter")) {
        for (int i = 0; i < IM_ARRAYSIZE(filter_options); i++) {
            if (ImGui::Selectable(filter_options[i])) {
                selected_filter = i;
            }
        }

        ImGui::EndPopup();
    }

    ImGui::PopFont();
    ImGui::PopFont();

    ImGui::Separator();

    // list (group by first letter)

    // Reserve enough left-over height for 1 separator + 1 input text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("Friends List", ImVec2(0, -footer_height_to_reserve),
                      false, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(4, 1));  // Tighten spacing

    const float TextPadding = 8.0f;

    for (auto user : m_friends) {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + TextPadding);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + TextPadding);

        ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(28, 28));
        ImGui::SameLine();

        ImGui::BeginGroup();
        ImGui::PushFont(Walnut::Application::GetFont("Bold"));

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

        ImGui::Text(user.get_fullname().c_str());
        ImGui::PopFont();

        ImGui::SameLine();

        ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

        // draw 4 icons
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 160);

        // profile
        if (ImGui::Button(ICON_FA_USER)) {
            m_profile_user = user;
            m_view_profile = true;
        }

        ImGui::SameLine();

        // message

        if (ImGui::Button(ICON_FA_COMMENT)) {
            int dm_id = DirectMessageManager::getInstance()
                            .get_direct_message(m_current_user.get_user_id(),
                                                user.get_user_id())
                            .value()
                            .get_dm_id();
            ToDirectMessage(dm_id);
        }

        ImGui::SameLine();

        // block

        if (ImGui::Button(ICON_FA_BAN)) {
            // ...
        }

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text(("Block " + user.get_fullname()).c_str());
            ImGui::EndTooltip();
        }

        ImGui::SameLine();

        // unfriend

        if (ImGui::Button(ICON_FA_USER_TIMES)) {
            // open a modal to clarify
            ImGui::OpenPopup("##Unfriend");
        }

        if (ImGui::BeginPopupModal("##Unfriend", NULL,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text(("Are you sure you want to unfriend " +
                         user.get_fullname() + "?")
                            .c_str());

            ImGui::Separator();

            if (ImGui::Button("Yes", ImVec2(120, 0))) {
                // send unfriend request
                Walnut::BufferStreamWriter stream(m_scratch_buffer);
                stream.WriteRaw< PacketType >(PacketType::Unfriend);
                stream.WriteRaw< uint32_t >(user.get_user_id());

                m_client->SendBuffer(stream.GetBuffer());

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("No", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text(("Unfriend " + user.get_fullname()).c_str());
            ImGui::EndTooltip();
        }

        ImGui::PopFont();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

        ImGui::Separator();
        ImGui::EndGroup();
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
}

void ClientLayer::UI_Groups() {
    ImGui::PopFont();

    ImGui::Separator();

    static std::string search;

    ImGui::Text("Search");

    ImGui::SameLine();

    ImGui::SetNextItemWidth(180);

    ImGui::InputText("##Search", &search);

    ImGui::SameLine();

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
    ImGui::PushFont(Walnut::Application::GetFont("Default"));

    static int selected_sort = 0;

    const char* sort_options[] = {ICON_FA_ARROW_UP " Name (A-Z)",
                                  ICON_FA_ARROW_DOWN " Name (Z-A)"};

    if (ImGui::Button(sort_options[selected_sort])) {
        ImGui::OpenPopup("Sort");
    }

    if (ImGui::BeginPopup("Sort")) {
        for (int i = 0; i < IM_ARRAYSIZE(sort_options); i++) {
            if (ImGui::Selectable(sort_options[i])) {
                selected_sort = i;
            }
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    static int selected_filter = 0;

    const char* filter_options[] = {ICON_FA_FILTER " All",
                                    ICON_FA_FILTER " Group that I am an admin"};

    if (ImGui::Button((selected_filter == -1
                           ? "None"
                           : filter_options[selected_filter]))) {
        ImGui::OpenPopup("Filter");
    }

    if (ImGui::BeginPopup("Filter")) {
        for (int i = 0; i < IM_ARRAYSIZE(filter_options); i++) {
            if (ImGui::Selectable(filter_options[i])) {
                selected_filter = i;
            }
        }

        ImGui::EndPopup();
    }

    ImGui::PopFont();
    ImGui::PopFont();

    ImGui::Separator();

    // list (group by first letter)

    // Reserve enough left-over height for 1 separator + 1 input text
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
    const float footer_height_to_reserve =
        ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("Groups", ImVec2(0, -footer_height_to_reserve), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(4, 1));  // Tighten spacing

    const float TextPadding = 8.0f;

    ImGui::SetCursorPosY(TextPadding);

    ImGui::SetCursorPosX(TextPadding);

    ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(28, 28));
    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::PushFont(Walnut::Application::GetFont("Bold"));

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

    ImGui::Text("Nguyen Van A");
    ImGui::PopFont();

    ImGui::SameLine();

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 50);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 5);

    if (ImGui::Button(ICON_FA_ELLIPSIS_V)) {
        ImGui::OpenPopup("##Group Options");
    }

    ImGui::PopFont();

    static int group_option = 0;

    const char* group_options[] = {"Info", "Mute", "Leave Group"};

    if (ImGui::BeginPopup("##Group Options")) {
        for (int i = 0; i < IM_ARRAYSIZE(group_options); i++) {
            if (ImGui::Selectable(group_options[i])) {
                group_option = i;
            }
            if (i < IM_ARRAYSIZE(group_options) - 1) ImGui::Separator();
        }

        ImGui::EndPopup();
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

    ImGui::Separator();

    ImGui::EndGroup();

    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
}

void ClientLayer::UI_FriendsRequest() {
    ImGui::PopFont();

    // this tab will contain 3 collapsing headers (Friend requests,
    // Suggestions, Add friends)

    if (ImGui::CollapsingHeader("Friend requests")) {
        // avatar + name + accept + decline

        for (int i = 0; i < m_pending_friends.size(); i++) {
            const User& user = m_pending_friends[i];

            UI_FriendItem(user, m_pending_friend_notes[i]);
        }
    }

    if (ImGui::CollapsingHeader("Suggestions")) {
        // avatar + name + add friend

        ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));

        ImGui::SameLine();

        ImGui::BeginGroup();

        ImGui::PushFont(Walnut::Application::GetFont("Bold"));

        ImGui::Text("Nguyen Van A");

        ImGui::PopFont();

        // text input to add note

        std::string note;

        ImGui::InputText("##note", &note);

        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 80);

        ImGui::Button(ICON_FA_USER_CIRCLE);

        ImGui::SameLine();

        ImGui::Button(ICON_FA_USER_PLUS);

        ImGui::PopFont();

        ImGui::Separator();
    }

    if (ImGui::CollapsingHeader("Add friends")) {
        // display a search bar that you can use to search for friends
        // when you enter a name, it will display a list of friends that
        // match

        // search bar

        static std::string search;

        ImGui::Text("Search");

        ImGui::SameLine();

        ImGui::SetNextItemWidth(180);

        ImGui::InputText("##Search", &search);

        ImGui::SameLine();

        ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

        ImGui::Button(ICON_FA_SEARCH);

        ImGui::PopFont();

        ImGui::Separator();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
        const float footer_height_to_reserve =
            ImGui::GetStyle().ItemSpacing.y +
            ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("Friends List", ImVec2(0, -footer_height_to_reserve),
                          false, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                            ImVec2(4, 1));  // Tighten spacing

        const float TextPadding = 8.0f;

        std::vector< User > users = UserManager::getInstance().get_users();
        for (auto user : users) {
            if (user.get_user_id() == m_current_user.get_user_id()) continue;
            // if user is a friend, skip
            if (std::find(m_friends.begin(), m_friends.end(), user) !=
                m_friends.end())
                continue;

            ImGui::SetCursorPosX(TextPadding + ImGui::GetCursorPosX());

            ImGui::SetCursorPosY(TextPadding + ImGui::GetCursorPosY());

            UI_FriendItem(user, m_user_notes[user.get_user_id()]);
        }

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
}

void ClientLayer::UI_FriendItem(const User& user, std::string& note) {
    ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));

    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::BeginGroup();

    ImGui::PushFont(Walnut::Application::GetFont("Bold"));

    ImGui::Text(user.get_fullname().c_str());

    ImGui::PopFont();

    ImGui::InputText(("note##" + std::to_string(user.get_user_id())).c_str(),
                     &note);

    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

    if (std::find(m_pending_friends.begin(), m_pending_friends.end(), user) !=
        m_pending_friends.end()) {
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 115);

        if (ImGui::Button(
                (ICON_FA_USER_CIRCLE "##" + std::to_string(user.get_user_id()))
                    .c_str())) {
            m_view_profile = true;
            m_profile_user = user;
        }

        ImGui::SameLine();

        if (ImGui::Button(
                (ICON_FA_CHECK "##" + std::to_string(user.get_user_id()))
                    .c_str())) {
            // accept friend request
            Walnut::BufferStreamWriter stream(m_scratch_buffer);

            stream.WriteRaw< PacketType >(PacketType::AcceptFriend);

            stream.WriteRaw< int64_t >(user.get_user_id());

            m_client->SendBuffer(stream.GetBuffer());
        }

        ImGui::SameLine();

        if (ImGui::Button(
                (ICON_FA_TIMES "##" + std::to_string(user.get_user_id()))
                    .c_str())) {
            // decline friend request
            Walnut::BufferStreamWriter stream(m_scratch_buffer);

            stream.WriteRaw< PacketType >(PacketType::DeclineFriend);

            stream.WriteRaw< int64_t >(user.get_user_id());

            m_client->SendBuffer(stream.GetBuffer());
        }
    } else {
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 80);

        if (ImGui::Button(
                (ICON_FA_USER_CIRCLE "##" + std::to_string(user.get_user_id()))
                    .c_str())) {
            m_view_profile = true;
            m_profile_user = user;
        }

        ImGui::SameLine();

        if (ImGui::Button(
                (ICON_FA_USER_PLUS "##" + std::to_string(user.get_user_id()))
                    .c_str())) {
            // add friend
            Walnut::BufferStreamWriter stream(m_scratch_buffer);

            stream.WriteRaw< PacketType >(PacketType::AddFriend);

            stream.WriteRaw< int64_t >(user.get_user_id());

            stream.WriteString(note);

            m_client->SendBuffer(stream.GetBuffer());
        }
    }

    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

    ImGui::Separator();

    ImGui::EndGroup();
}

void ClientLayer::UI_Settings() {
    UI_MainTab();

    ImGui::SetNextWindowSize(ImVec2(350, 800), ImGuiCond_FirstUseEver);

    bool open = true;
    ImGui::Begin("Friends Tab", &open,
                 ImGuiWindowFlags_NoDecoration | ImGuiDockNodeFlags_NoTabBar);

    static int style_idx = -1;
    if (ImGui::Combo("Themes##Selector", &style_idx, "Dark\0Light\0Classic\0")) {
        switch (style_idx) {
            case 0:
                ImGui::StyleColorsDark();
                break;
            case 1:
                ImGui::StyleColorsLight();
                break;
            case 2:
                ImGui::StyleColorsClassic();
                break;
        }
    }

    ImGui::End();
}

void ClientLayer::OnConnected() {}

void ClientLayer::OnDisconnected() { m_client->Disconnect(); }

void ClientLayer::OnDataReceived(const Walnut::Buffer buffer) {
    Walnut::BufferStreamReader stream(buffer);

    Walnut::BufferStreamWriter out_stream(m_scratch_buffer);

    PacketType type;
    stream.ReadRaw< PacketType >(type);

    bool success;
    std::vector< User > users;
    std::vector< GroupMessage > group_messages;
    std::vector< DirectMessage > direct_messages;
    std::vector< Conversation > conversations;
    std::vector< Message > messages;

    int dm_id;
    int user_id;
    int msg_id_tmp;
    int time_tmp;
    Message msg_tmp;
    std::string nickname_tmp;

    std::cout << PacketTypeToString(type) << std::endl;

    switch (type) {
        case PacketType::ClientRegisterRequest:
        case PacketType::ClientLoginRequest:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadObject(m_current_user);

            SendPacket(PacketType::RetrieveAllUsers);
            SendPacket(PacketType::RetrieveAllFriends);
            SendPacket(PacketType::RetrievePendingFriendRequests);
            SendPacket(PacketType::RetrieveAllGroups);
            SendPacket(PacketType::RetrieveAllDMs);

            m_current_tab = Tab::Chat;

            m_logged_in = true;

            if (m_remember_me)
                SaveCredentials();
            else
                DeleteCredentials();

            break;
        case PacketType::ClientLogoutRequest:

            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }
            m_logged_in = false;

            m_current_tab = Tab::Default;

            DeleteCredentials();

            break;
        case PacketType::RetrieveAllUsers:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadArray(users);

            UserManager::getInstance().load_users(users);

            break;
        case PacketType::RetrieveAllFriends:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadArray(m_friends);

            for (auto user : m_friends) {
                std::cout << user << std::endl;
            }

            break;
        case PacketType::AddFriend:
        case PacketType::DeclineFriend:
        case PacketType::Unfriend:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }
            break;

        case PacketType::AcceptFriend:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            SendPacket(PacketType::RetrieveAllDMs);

            break;

        case PacketType::RetrievePendingFriendRequests:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadArray(m_pending_friends);

            stream.ReadArray(m_pending_friend_notes);

            break;
        case PacketType::RetrieveAllGroups:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadArray(group_messages);
            stream.ReadArray(conversations);
            stream.ReadArray(messages);

            GroupMessageManager::getInstance().load_group_messages(
                group_messages);
            ConversationManager::getInstance().insert_conversations(
                conversations);
            MessageManager::getInstance().insert_messages(messages);

            std::cout << "Group" << std::endl;

            for (auto group : group_messages) {
                std::cout << group << std::endl;
            }

            break;
        case PacketType::RetrieveAllDMs:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadArray(direct_messages);
            stream.ReadArray(conversations);
            stream.ReadArray(messages);

            DirectMessageManager::getInstance().load_direct_messages(
                direct_messages);
            ConversationManager::getInstance().insert_conversations(
                conversations);
            MessageManager::getInstance().insert_messages(messages);

            break;
        case PacketType::SendDirectMessage:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadRaw< int >(dm_id);
            stream.ReadObject(msg_tmp);

            DirectMessageManager::getInstance().add_message(dm_id, msg_tmp);

            if (m_chat.type == 0 && m_chat.id == dm_id) {
                ToDirectMessage(dm_id);
            }

            break;
        case PacketType::SendGroupMessage:

            break;
        case PacketType::UpdateLastSeenDM:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadRaw< int >(dm_id);
            stream.ReadRaw< int >(user_id);
            stream.ReadRaw< int >(time_tmp);

            DirectMessageManager::getInstance().update_last_seen(
                dm_id, user_id,
                date::sys_seconds(std::chrono::seconds(time_tmp)));

            break;
        case PacketType::AddPinMessageDM:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadRaw< int >(dm_id);
            stream.ReadRaw< int >(msg_id_tmp);
            stream.ReadObject(msg_tmp);

            DirectMessageManager::getInstance().add_pin_message(dm_id,
                                                                msg_id_tmp);

            DirectMessageManager::getInstance().add_message(dm_id, msg_tmp);

            break;
        case PacketType::AddPinMessageGroup:

            break;
        case PacketType::RemovePinMessageDM:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;

                break;
            }

            stream.ReadRaw< int >(dm_id);
            stream.ReadRaw< int >(msg_id_tmp);
            stream.ReadObject(msg_tmp);

            DirectMessageManager::getInstance().remove_pin_message(dm_id,
                                                                   msg_id_tmp);

            DirectMessageManager::getInstance().add_message(dm_id, msg_tmp);

            break;
        case PacketType::RemovePinMessageGroup:

            break;
        case PacketType::DeleteMessageDM:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);
                std::cout << error_msg << std::endl;
                break;
            }

            stream.ReadRaw< int >(dm_id);
            stream.ReadRaw< int >(msg_id_tmp);

            DirectMessageManager::getInstance().delete_message(dm_id,
                                                               msg_id_tmp);

            break;
        case PacketType::DeleteMessageGroup:

            break;
        case PacketType::UpdateNicknameDM:
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);
                std::cout << error_msg << std::endl;
                break;
            }

            stream.ReadRaw< int >(dm_id);
            stream.ReadRaw< int >(user_id);
            stream.ReadString(nickname_tmp);
            stream.ReadObject(msg_tmp);

            DirectMessageManager::getInstance().update_nickname(dm_id, user_id,
                                                                nickname_tmp);

            // add an announcement message

            DirectMessageManager::getInstance().add_message(dm_id, msg_tmp);
            break;
        case PacketType::UpdateNicknameGroup:

            break;
    }
}

void ClientLayer::SaveCredentials() {
    YAML::Emitter out;

    out << YAML::BeginMap;

    out << YAML::Key << "server_ip" << YAML::Value << m_server_ip;

    out << YAML::Key << "username" << YAML::Value
        << m_current_user.get_username();

    out << YAML::Key << "password" << YAML::Value
        << m_current_user.get_password();

    out << YAML::EndMap;

    std::ofstream fout(m_saved_credentials_file);

    fout << out.c_str();

    fout.close();
}

void ClientLayer::LoadCredentials() {
    std::ifstream fin(m_saved_credentials_file);

    if (!fin.is_open()) return;

    YAML::Node doc = YAML::Load(fin);

    m_server_ip = doc["server_ip"].as< std::string >();
    std::string username = doc["username"].as< std::string >();
    std::string password = doc["password"].as< std::string >();

    m_current_user.set_username(username);
    m_current_user.set_password(password);

    fin.close();

    ConnectToServer();
    m_has_credentials = true;
    m_remember_me = true;
}

void ClientLayer::DeleteCredentials() {
    try {
        std::filesystem::remove(m_saved_credentials_file);
    } catch (std::filesystem::filesystem_error& e) {
        // std::cout << e.what() << std::endl;
    }
}

void ClientLayer::ConnectToServer() {
    if (Walnut::Utils::IsValidIPAddress(m_server_ip)) {
        m_client->ConnectToServer(m_server_ip);
    } else {
        // Try resolve domain name
        auto ipTokens = Walnut::Utils::SplitString(
            m_server_ip,
            ':');  // [0] == hostname, [1] (optional) == port
        std::string serverIP = Walnut::Utils::ResolveDomainName(ipTokens[0]);
        if (ipTokens.size() != 2)
            serverIP = fmt::format("{}:{}", serverIP,
                                   8192);  // Add default port if hostname
                                           // doesn't contain port
        else
            serverIP = fmt::format("{}:{}", serverIP,
                                   ipTokens[1]);  // Add specified port

        m_client->ConnectToServer(serverIP);
    }
}

void ClientLayer::LoginToServer() {
    Walnut::BufferStreamWriter stream(m_scratch_buffer);
    stream.WriteRaw< PacketType >(PacketType::ClientLoginRequest);
    stream.WriteString(m_current_user.m_username);
    stream.WriteString(m_current_user.m_password);

    m_client->SendBuffer(stream.GetBuffer());

    ImGui::CloseCurrentPopup();
}

void ClientLayer::ToDirectMessage(int dm_id) {
    m_chat.type = 0;
    m_chat.id = dm_id;

    m_current_tab = Tab::Chat;

    Walnut::Buffer scratch_buffer;
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);

    stream.WriteRaw< PacketType >(PacketType::UpdateLastSeenDM);
    stream.WriteRaw< int >(dm_id);

    m_client->SendBuffer(stream.GetBuffer());
}

void ClientLayer::ToGroupMessage(int group_id) {
    m_chat.type = 0;
    m_chat.id = group_id;

    m_current_tab = Tab::Chat;
}

void ClientLayer::PinMessage(int message_id) {
    Walnut::Buffer scratch_buffer;
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);

    if (!m_chat.type) {
        stream.WriteRaw< PacketType >(PacketType::AddPinMessageDM);
    } else {
        stream.WriteRaw< PacketType >(PacketType::AddPinMessageGroup);
    }
    stream.WriteRaw< int >(m_chat.id);
    stream.WriteRaw< int >(message_id);

    m_client->SendBuffer(stream.GetBuffer());
}

void ClientLayer::UnpinMessage(int message_id) {
    Walnut::Buffer scratch_buffer;
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);

    if (!m_chat.type) {
        stream.WriteRaw< PacketType >(PacketType::RemovePinMessageDM);
    } else {
        stream.WriteRaw< PacketType >(PacketType::RemovePinMessageGroup);
    }
    stream.WriteRaw< int >(m_chat.id);
    stream.WriteRaw< int >(message_id);

    m_client->SendBuffer(stream.GetBuffer());
}

void ClientLayer::DeleteMessage(int message_id) {
    Walnut::Buffer scratch_buffer;
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);

    if (!m_chat.type) {
        stream.WriteRaw< PacketType >(PacketType::DeleteMessageDM);
    } else {
        stream.WriteRaw< PacketType >(PacketType::DeleteMessageGroup);
    }
    stream.WriteRaw< int >(m_chat.id);
    stream.WriteRaw< int >(message_id);

    m_client->SendBuffer(stream.GetBuffer());
}

void ClientLayer::UpdateNickname(int user_id, std::string_view nickname) {
    Walnut::Buffer scratch_buffer;
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);

    if (!m_chat.type) {
        stream.WriteRaw< PacketType >(PacketType::UpdateNicknameDM);
    } else {
        stream.WriteRaw< PacketType >(PacketType::UpdateNicknameGroup);
    }
    stream.WriteRaw< int >(m_chat.id);
    stream.WriteRaw< int >(user_id);
    stream.WriteString(nickname);

    m_client->SendBuffer(stream.GetBuffer());
}

void ClientLayer::SendChatMessage(std::string_view message) {
    if (!message.length()) return;

    Walnut::Buffer scratch_buffer;
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);

    if (!m_chat.type) {
        stream.WriteRaw< PacketType >(PacketType::SendDirectMessage);
    } else {
        stream.WriteRaw< PacketType >(PacketType::SendGroupMessage);
    }
    stream.WriteRaw< int >(m_chat.id);
    stream.WriteString(message);

    m_client->SendBuffer(stream.GetBuffer());
}

void ClientLayer::SendPacket(PacketType packet_type) {
    Walnut::Buffer scratch_buffer;
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter stream(scratch_buffer);
    stream.WriteRaw< PacketType >(packet_type);

    std::cout << "Sending packet " << PacketTypeToString(packet_type)
              << std::endl;

    m_client->SendBuffer(stream.GetBuffer());
}
