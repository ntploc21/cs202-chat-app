#include "DMHandler.hpp"

#include <iostream>

#include "../ClientManager.hpp"
#include "ConversationManager.hpp"
#include "DirectMessageManager.hpp"
#include "UserManager.hpp"
#include "Walnut/Serialization/BufferStream.h"

RetrieveAllDMsHandler::RetrieveAllDMsHandler() {}
SendDirectMessageHandler::SendDirectMessageHandler() {}
UpdateLastSeenDMHandler::UpdateLastSeenDMHandler() {}
AddPinMessageDMHandler::AddPinMessageDMHandler() {}
RemovePinMessageDMHandler::RemovePinMessageDMHandler() {}
DeleteMessageDMHandler::DeleteMessageDMHandler() {}
UpdateNicknameDMHandler::UpdateNicknameDMHandler() {}

void RetrieveAllDMsHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                       const PacketType packet_type,
                                       Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    std::vector< DirectMessage > dms =
        DirectMessageManager::getInstance().get_direct_messages(
            client.get_user_id());

    std::vector< Conversation > conversations;

    for (auto& dm : dms) {
        auto tmp = ConversationManager::getInstance().get_conversation(
            dm.get_conversation_id());

        if (!tmp.has_value()) continue;

        conversations.push_back(tmp.value());
    }

    std::vector< Message > messages;

    for (auto& conv : conversations) {
        std::vector< Message > messages_dm = conv.get_messages();
        messages.insert(messages.end(), messages_dm.begin(), messages_dm.end());
    }

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(8192);

    Walnut::BufferStreamWriter out_stream{scratch_buffer};
    out_stream.WriteRaw< PacketType >(PacketType::RetrieveAllDMs);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteArray(dms);
    out_stream.WriteArray(conversations);
    out_stream.WriteArray(messages);

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}

void SendDirectMessageHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                          const PacketType packet_type,
                                          Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int dm_id;
    std::string message;

    stream.ReadRaw< int >(dm_id);
    stream.ReadString(message);

    auto dm = DirectMessageManager::getInstance().get_direct_message(dm_id);

    if (!dm.has_value()) {
        SendErrorDM(m_server, client_info, packet_type, "DM not exists.");
    }

    auto msg = dm.value().send_message(client.get_user_id(), message);

    if (!msg.has_value()) {
        SendErrorDM(m_server, client_info, packet_type,
                    "Failed to send message.");
    }

    auto client_friend = ClientManager::getInstance().getClientID(
        dm.value().get_user_id_1() == client.get_user_id()
            ? dm.value().get_user_id_2()
            : dm.value().get_user_id_1());

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteRaw< int >(dm_id);
    out_stream.WriteObject(msg.value());

    std::cout << client_friend.has_value() << std::endl;

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));

    if (client_friend.has_value()) {
        m_server->SendBufferToClient(
            client_friend.value(),
            Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
    }
}

void UpdateLastSeenDMHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                         const PacketType packet_type,
                                         Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int dm_id;
    stream.ReadRaw< int >(dm_id);

    auto dm = DirectMessageManager::getInstance().get_direct_message(dm_id);

    if (!dm.has_value()) {
        SendErrorDM(m_server, client_info, packet_type, "DM not exists.");
        return;
    }

    date::sys_seconds prev_last_seen =
        dm.value().get_user_last_seen_at(client.get_user_id());

    auto prev_msg_created =
        ConversationManager::getInstance()
            .get_conversation(dm.value().get_conversation_id())
            .value()
            .get_last_message();

    if (prev_msg_created.has_value()) {
        date::sys_seconds prev_msg_created_at =
            prev_msg_created.value().m_created_at;

        if (prev_last_seen >= prev_msg_created_at) {
            SendErrorDM(m_server, client_info, packet_type,
                        "Already up-to-date.");
            return;
        }
    }

    DirectMessageManager::getInstance().update_last_seen(
        dm_id, client.get_user_id(), prev_last_seen);

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteRaw< int >(dm_id);
    out_stream.WriteRaw< int >(client.get_user_id());
    out_stream.WriteRaw< int >(prev_last_seen.time_since_epoch().count());

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));

    auto client_friend = ClientManager::getInstance().getClientID(
        dm.value().get_user_id_1() == client.get_user_id()
            ? dm.value().get_user_id_2()
            : dm.value().get_user_id_1());

    if (client_friend.has_value()) {
        m_server->SendBufferToClient(
            client_friend.value(),
            Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
    }
}

void AddPinMessageDMHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                        const PacketType packet_type,
                                        Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int dm_id;
    stream.ReadRaw< int >(dm_id);

    int message_id;
    stream.ReadRaw< int >(message_id);

    if (!DirectMessageManager::getInstance().add_pin_message(dm_id,
                                                             message_id)) {
        SendErrorDM(m_server, client_info, packet_type,
                    "Failed to add pin message.");
        return;
    }

    // add an announcement "<<User username>> pinned a message."
    std::string announcement_msg = UserManager::getInstance()
                                       .get_user(client.get_user_id())
                                       .value()
                                       .get_fullname() +
                                   " pinned a message.";

    auto announcement = DirectMessageManager::getInstance().send_announcement(
        dm_id, client.get_user_id(), announcement_msg);

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteRaw< int >(dm_id);
    out_stream.WriteRaw< int >(message_id);
    out_stream.WriteObject(announcement.value());

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));

    auto client_friend = ClientManager::getInstance().getClientID(
        DirectMessageManager::getInstance()
                    .get_direct_message(dm_id)
                    .value()
                    .get_user_id_1() == client.get_user_id()
            ? DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_2()
            : DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_1());

    if (client_friend.has_value()) {
        m_server->SendBufferToClient(
            client_friend.value(),
            Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
    }
}

void RemovePinMessageDMHandler::handleImpl(
    const Walnut::ClientInfo& client_info, const PacketType packet_type,
    Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int dm_id;
    stream.ReadRaw< int >(dm_id);

    int message_id;
    stream.ReadRaw< int >(message_id);

    if (!DirectMessageManager::getInstance().remove_pin_message(dm_id,
                                                                message_id)) {
        SendErrorDM(m_server, client_info, packet_type,
                    "Failed to remove pin message.");
        return;
    }

    // add an announcement "<<User username>> unpinned a message."
    std::string announcement_msg = UserManager::getInstance()
                                       .get_user(client.get_user_id())
                                       .value()
                                       .get_fullname() +
                                   " unpinned a message.";

    auto announcement = DirectMessageManager::getInstance().send_announcement(
        dm_id, client.get_user_id(), announcement_msg);

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteRaw< int >(dm_id);
    out_stream.WriteRaw< int >(message_id);
    out_stream.WriteObject(announcement.value());

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));

    auto client_friend = ClientManager::getInstance().getClientID(
        DirectMessageManager::getInstance()
                    .get_direct_message(dm_id)
                    .value()
                    .get_user_id_1() == client.get_user_id()
            ? DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_2()
            : DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_1());

    if (client_friend.has_value()) {
        m_server->SendBufferToClient(
            client_friend.value(),
            Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
    }
}

void DeleteMessageDMHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                        const PacketType packet_type,
                                        Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int dm_id;
    stream.ReadRaw< int >(dm_id);

    int message_id;
    stream.ReadRaw< int >(message_id);

    if (!DirectMessageManager::getInstance().delete_message(dm_id,
                                                            message_id)) {
        SendErrorDM(m_server, client_info, packet_type,
                    "Failed to delete message.");
        return;
    }

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);
    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteRaw< int >(dm_id);
    out_stream.WriteRaw< int >(message_id);

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));

    auto client_friend = ClientManager::getInstance().getClientID(
        DirectMessageManager::getInstance()
                    .get_direct_message(dm_id)
                    .value()
                    .get_user_id_1() == client.get_user_id()
            ? DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_2()
            : DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_1());

    if (client_friend.has_value()) {
        m_server->SendBufferToClient(
            client_friend.value(),
            Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
    }
}

void UpdateNicknameDMHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                         const PacketType packet_type,
                                         Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    int dm_id;
    stream.ReadRaw< int >(dm_id);

    int user_id;
    stream.ReadRaw< int >(user_id);

    std::string nickname;
    stream.ReadString(nickname);

    if (!DirectMessageManager::getInstance().update_nickname(dm_id, user_id,
                                                             nickname)) {
        SendErrorDM(m_server, client_info, packet_type,
                    "Failed to update nickname.");
        return;
    }

    // add an announcement "<<User username>> updated <<Who>> nickname."

    std::string announcement_msg =
        UserManager::getInstance()
            .get_user(client.get_user_id())
            .value()
            .get_fullname() +
        " updated " +
        UserManager::getInstance().get_user(user_id).value().get_fullname() +
        "'s nickname to " + nickname + ".";

    auto announcement = DirectMessageManager::getInstance().send_announcement(
        dm_id, client.get_user_id(), announcement_msg);

    Walnut::Buffer scratch_buffer{};
    scratch_buffer.Allocate(1024);

    Walnut::BufferStreamWriter out_stream(scratch_buffer);

    out_stream.WriteRaw< PacketType >(packet_type);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteRaw< int >(dm_id);
    out_stream.WriteRaw< int >(user_id);
    out_stream.WriteString(nickname);
    out_stream.WriteObject(announcement.value());

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));

    auto client_friend = ClientManager::getInstance().getClientID(
        DirectMessageManager::getInstance()
                    .get_direct_message(dm_id)
                    .value()
                    .get_user_id_1() == client.get_user_id()
            ? DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_2()
            : DirectMessageManager::getInstance()
                  .get_direct_message(dm_id)
                  .value()
                  .get_user_id_1());

    if (client_friend.has_value()) {
        m_server->SendBufferToClient(
            client_friend.value(),
            Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
    }
}

void SendErrorDM(std::shared_ptr< Walnut::Server > server,
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