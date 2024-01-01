#include "ClientLayer.hpp"

#include <imgui.h>

#include <iostream>

#include "IconsFontAwesome5.h"
#include "ServerPacket.hpp"
#include "Session.hpp"
#include "Walnut/Application.h"
#include "Walnut/ApplicationGUI.h"
#include "Walnut/Networking/NetworkingUtils.h"
#include "Walnut/Serialization/BufferStream.h"
#include "Walnut/UI/UI.h"
#include "Walnut/Utils/StringUtils.h"
#include "misc/cpp/imgui_stdlib.h"

void ClientLayer::OnAttach() {
    m_scratch_buffer.Allocate(8192);

    m_client = std::make_unique< Walnut::Client >();
    m_client->SetServerConnectedCallback([this]() { OnConnected(); });
    m_client->SetServerDisconnectedCallback([this]() { OnDisconnected(); });
    m_client->SetDataReceivedCallback(
        [this](const Walnut::Buffer data) { OnDataReceived(data); });

    // ImGui::StyleColorsLight();

    m_test_avt = std::make_shared< Walnut::Image >("Image/test.png");
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

    if (m_current_tab == Tab::Chat) {
        UI_MainChat();
    }

    if (m_current_tab == Tab::Contact) {
        UI_Friends();
    }

    if (m_current_tab == Tab::Settings) {
        ImGui::Text("Settings");
    }
}

bool ClientLayer::IsConnected() const {
    return m_client->GetConnectionStatus() ==
           Walnut::Client::ConnectionStatus::Connected;
}

void ClientLayer::OnDisconnectButton() { m_client->Disconnect(); }

void ClientLayer::UI_ConnectionModal() {
    if (m_client->GetConnectionStatus() ==
        Walnut::Client::ConnectionStatus::Connected)
        return;

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
            if (Walnut::Utils::IsValidIPAddress(m_server_ip)) {
                m_client->ConnectToServer(m_server_ip);
            } else {
                // Try resolve domain name
                auto ipTokens = Walnut::Utils::SplitString(
                    m_server_ip,
                    ':');  // [0] == hostname, [1] (optional) == port
                std::string serverIP =
                    Walnut::Utils::ResolveDomainName(ipTokens[0]);
                if (ipTokens.size() != 2)
                    serverIP =
                        fmt::format("{}:{}", serverIP,
                                    8192);  // Add default port if hostname
                                            // doesn't contain port
                else
                    serverIP = fmt::format("{}:{}", serverIP,
                                           ipTokens[1]);  // Add specified port

                m_client->ConnectToServer(serverIP);
            }
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
        ImGui::InputText("##username", &current_user.m_username);

        ImGui::Text("Password");
        ImGui::InputText("##password", &current_user.m_password,
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

        if (ImGui::Button("Sign in")) {
            Walnut::BufferStreamWriter stream(m_scratch_buffer);
            stream.WriteRaw< PacketType >(PacketType::ClientLoginRequest);
            stream.WriteString(current_user.m_username);
            stream.WriteString(current_user.m_password);

            m_client->SendBuffer(stream.GetBuffer());

            ImGui::CloseCurrentPopup();
        }

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
        ImGui::InputText("##fullname", &current_user.m_fullname);

        ImGui::Text("Username");
        ImGui::InputText("##username", &current_user.m_username);

        ImGui::Text("Password");
        ImGui::InputText("##password", &current_user.m_password,
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
            stream.WriteString(current_user.m_username);
            stream.WriteString(current_user.m_password);
            stream.WriteString(current_user.m_fullname);

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

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesomeBig"));

    // center the icon in the selectable
    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.7f));
    // margin 0
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    // center the icon vertically in the selectable

    if(ImGui::Selectable(ICON_FA_COMMENT, (m_current_tab == Tab::Chat), ImGuiSelectableFlags_None,
                      ImVec2(50, 50))) {
        m_current_tab = Tab::Chat;
    }

    if(ImGui::Selectable(ICON_FA_ADDRESS_BOOK, (m_current_tab == Tab::Contact), ImGuiSelectableFlags_None,
                      ImVec2(50, 50))) {
        m_current_tab = Tab::Contact;
    }

    if(ImGui::Selectable(ICON_FA_COG, (m_current_tab == Tab::Settings), ImGuiSelectableFlags_None,
                      ImVec2(50, 50))) {
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

            {
                ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
                ImGui::SameLine();

                ImGui::BeginGroup();

                ImGui::PushFont(Walnut::Application::GetFont("Bold"));
                ImGui::Text("Nguyen Van A");
                ImGui::PopFont();

                ImGui::Text("Hello, how are you?");
                ImGui::SameLine();

                ImGui::PushFont(Walnut::Application::GetFont("Italic"));
                ImGui::BulletText("(10:00 AM)");
                ImGui::PopFont();

                ImGui::EndGroup();
            }

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Others")) {
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

    // add a header that contains the name of the avatar, the name and the
    // status

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));

    ImGui::PopStyleVar(2);

    ImGui::SameLine();

    ImGui::BeginGroup();

    ImGui::PushFont(Walnut::Application::GetFont("Bold"));

    ImGui::Text("Nguyen Van A");

    ImGui::PopFont();

    // draw colorful center dot to indicate status

    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

    ImGui::BulletText("");

    ImGui::PopStyleColor();

    ImGui::Text("Last seen 10:00 AM");

    ImGui::EndGroup();

    ImGui::Separator();

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

        const float TextPadding = 8.0f;

        ImGui::SetCursorPosY(TextPadding);

        for (int i = 0; i < 5; i++) {
            /*if (!m_Filter.PassFilter(m_MessageHistory[i].Message.c_str()))
                continue;

            ImGui::SetCursorPosX(TextPadding);

            // Normally you would store more information in your item than just
            // a string. (e.g. make Items[] an array of structure, store
            // color/type etc.)
            ImGui::PushStyleColor(ImGuiCol_Text,
                                  ImColor(m_MessageHistory[i].Color).Value);
            ImVec4 finalColor = ImColor(m_MessageHistory[i].Color).Value;
            if (!m_MessageHistory[i].Tag.empty()) {
                ImGui::PushFont(Application::GetFont("Bold"));
                ImGui::TextUnformatted(m_MessageHistory[i].Tag.c_str());
                ImGui::PopFont();
                ImGui::SameLine(0.0f, TextPadding);
            }

            if (m_MessageHistory[i].Italic)
                ImGui::PushFont(Application::GetFont("Italic"));

            ImGui::TextUnformatted(m_MessageHistory[i].Message.c_str());

            if (m_MessageHistory[i].Italic) ImGui::PopFont();

            ImGui::PopStyleColor();*/

            // every message has an avatar, a name, a message and a time (for
            // the most recent message group) implement this to look similar to
            // messenger (the most recent message group should be at the bottom)
            // if the messages are ours, they should be on the right, otherwise,
            // they should be on the left if the messages are ours, they should
            // be in blue, otherwise, they should be in white

            ImGui::SetCursorPosX(TextPadding);

            ImGui::BeginGroup();

            ImGui::NewLine();

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2 + 4 * 33);

            ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(28, 28));

            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();

            ImGui::PushFont(Walnut::Application::GetFont("Italic"));

            ImGui::Text("Nguyen Van A");

            ImGui::PopFont();

            // cover the whole width

            ImGui::Button("Hello, how are you?");
            if (ImGui::BeginPopupContextItem("##message")) {
                // add some options: copy, unsend for everyone, unsend for me,
                // forward, reply, pin

                // ... add later

                ImGui::EndPopup();
            }

            ImGui::Button("Hehe");

            ImGui::Button("Haha");

            ImGui::Button("Huhu");

            ImGui::Button("Hoho");

            // add time (nut using button)

            ImGui::PushFont(Walnut::Application::GetFont("Italic"));

            ImGui::Text("10:00 AM");

            ImGui::PopFont();

            ImGui::EndGroup();

            // message from you (only the message and the time)
            // messages are aligned to the right and in light blue
            // align in such a way that the the padding between the message and
            // the right side of the window is 8px

            ImGui::SetCursorPosX(ImGui::GetWindowSize().x -
                                 ImGui::CalcTextSize("Hello, how are you?").x -
                                 TextPadding - 25);

            // color the button, not the text

            ImGui::PushStyleColor(ImGuiCol_Button,
                                  ImVec4(0.0f, 0.0f, 1.0f, 0.1f));

            ImGui::Button("Hello, how are you?");

            ImGui::PopStyleColor();

            ImGui::PushFont(Walnut::Application::GetFont("Italic"));

            // set pos x

            ImGui::SetCursorPosX(ImGui::GetWindowSize().x -
                                 ImGui::CalcTextSize("10:00 AM").x -
                                 TextPadding - 10);

            ImGui::Text("10:00 AM");

            ImGui::PopFont();
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
        ImGuiInputTextFlags input_text_flags =
            ImGuiInputTextFlags_EnterReturnsTrue;

        const float sendButtonWidth = 100.0f;
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x -
                                sendButtonWidth - TextPadding);
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

    ImGui::End();
}

void ClientLayer::UI_Info() {
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
    std::string name = "Nguyen Van A";
    ImGui::SetCursorPosX(
        (ImGui::GetWindowSize().x - ImGui::CalcTextSize(name.c_str()).x) *
        0.5f);
    ImGui::PushFont(Walnut::Application::GetFont("Bold"));

    ImGui::Text(name.c_str());

    ImGui::PopFont();

    // center the status
    std::string status = "Active now";
    ImGui::SetCursorPosX(
        (ImGui::GetWindowSize().x - ImGui::CalcTextSize(status.c_str()).x) *
        0.5f);

    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), status.c_str());

    // draw three buttons profile, mute and search and it should all on the same
    // line

    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));

    // ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 30.0f);

    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 110) * 0.5f);

    ImGui::Button(ICON_FA_USER_CIRCLE);
    ImGui::SameLine();

    ImGui::Button(ICON_FA_BELL);
    ImGui::SameLine();

    ImGui::Button(ICON_FA_SEARCH);

    ImGui::PopStyleVar(2);

    ImGui::PopFont();

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Members")) {
        // avatar + name + nickname

        for (int i = 0; i < 5; i++) {
            ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
            ImGui::SameLine();

            ImGui::BeginGroup();

            ImGui::PushFont(Walnut::Application::GetFont("Bold"));

            ImGui::Text("Nguyen Van A");

            ImGui::PopFont();

            ImGui::PushFont(Walnut::Application::GetFont("Italic"));

            ImGui::Text("Nickname");

            ImGui::PopFont();

            ImGui::EndGroup();

            ImGui::SameLine();

            // add a ... button (aligned to right) to show more options

            ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));

            ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 50);

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);

            ImGui::Button(ICON_FA_ELLIPSIS_V);

            ImGui::PopFont();

            ImGui::Separator();
        }
    }

    // Pin messages
    if (ImGui::CollapsingHeader("Pin Messages")) {
        // avatar + name + message + time

        for (int i = 0; i < 5; i++) {
            ImGui::Image(m_test_avt->GetDescriptorSet(), ImVec2(50, 50));
            ImGui::SameLine();

            ImGui::BeginGroup();

            ImGui::PushFont(Walnut::Application::GetFont("Bold"));

            ImGui::Text("Nguyen Van A");

            ImGui::PopFont();

            ImGui::Text("Hello, how are you?");
            ImGui::SameLine();

            ImGui::PushFont(Walnut::Application::GetFont("Italic"));
            ImGui::BulletText("(10:00 AM)");
            ImGui::PopFont();

            ImGui::EndGroup();

            ImGui::Separator();
        }
    }

    // Shared media (images, files)
    if (ImGui::CollapsingHeader("Media and files")) {
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
    }

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
        UI_FriendsList();

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

void ClientLayer::UI_FriendsList() {

    // title

    // ImGui::PushFont(Walnut::Application::GetFont("Bold"));

    ImGui::Text(ICON_FA_ADDRESS_BOOK " Friends");
    
    ImGui::PopFont();

    // ImGui::PopFont();

    ImGui::Separator();

    // search

    static std::string search;

    ImGui::Text("Search");

    ImGui::SameLine();

    ImGui::SetNextItemWidth(180);

    ImGui::InputText("##Search", &search);

    ImGui::SameLine();

    // add a toggle button to sort by name (ascending or descending)
    // also, add a filter button to filter by online, all

    
    
    
    
    ImGui::PushFont(Walnut::Application::GetFont("FontAwesome"));
}

void ClientLayer::UI_Groups() {}

void ClientLayer::UI_FriendsRequest() {}

void ClientLayer::OnConnected() {}

void ClientLayer::OnDisconnected() { m_client->Disconnect(); }

void ClientLayer::OnDataReceived(const Walnut::Buffer buffer) {
    Walnut::BufferStreamReader stream(buffer);

    PacketType type;
    stream.ReadRaw< PacketType >(type);

    switch (type) {
        case PacketType::ClientRegisterRequest:
        case PacketType::ClientLoginRequest:
            bool success;
            stream.ReadRaw< bool >(success);

            if (!success) {
                std::string error_msg;
                stream.ReadString(error_msg);

                std::cout << error_msg << std::endl;
            } else {
                Session session;
                stream.ReadObject(session);

                std::cout << "Register success" << std::endl;
                std::cout << "Session id: " << session.get_session_id()
                          << std::endl;
                std::cout << "Session user id: " << session.get_user_id()
                          << std::endl;
            }
            break;
    }
}