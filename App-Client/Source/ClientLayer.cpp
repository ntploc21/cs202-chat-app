#include "ClientLayer.hpp"

#include <imgui.h>

#include <iostream>

#include "ServerPacket.hpp"
#include "Session.hpp"
#include "Walnut/Application.h"
#include "Walnut/Networking/NetworkingUtils.h"
#include "Walnut/Serialization/BufferStream.h"
#include "Walnut/UI/UI.h"
#include "Walnut/Utils/StringUtils.h"
#include "misc/cpp/imgui_stdlib.h"

void ClientLayer::OnAttach() {
    m_scratch_buffer.Allocate(1024);

    m_client = std::make_unique< Walnut::Client >();
    m_client->SetServerConnectedCallback([this]() { OnConnected(); });
    m_client->SetServerDisconnectedCallback([this]() { OnDisconnected(); });
    m_client->SetDataReceivedCallback(
        [this](const Walnut::Buffer data) { OnDataReceived(data); });

    // ImGui::StyleColorsLight();
}

void ClientLayer::OnDetach() {
    m_client->Disconnect();

    m_scratch_buffer.Release();
}

void ClientLayer::OnUIRender() {
    UI_ConnectionModal();
    UI_Login();
    UI_Register();
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
            // Send username
            /*Walnut::BufferStreamWriter stream(m_ScratchBuffer);
            stream.WriteRaw< PacketType >(PacketType::ClientConnectionRequest);
            stream.WriteRaw< uint32_t >(m_Color);  // Color
            stream.WriteString(m_Username);        // Username

            m_client->SendBuffer(stream.GetBuffer());

            SaveConnectionDetails(m_ConnectionDetailsFilePath);*/

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
        !m_login)
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
        m_login)
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
                std::cout << "Session id: " << session.get_session_id() << std::endl;
                std::cout << "Session user id: " << session.get_user_id()
						  << std::endl;

            }
            break;
    }
}
