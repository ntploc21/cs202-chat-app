#include "DMHandler.hpp"

#include <iostream>

#include "../ClientManager.hpp"
#include "ConversationManager.hpp"
#include "DirectMessageManager.hpp"
#include "Walnut/Serialization/BufferStream.h"

RetrieveAllDMsHandler::RetrieveAllDMsHandler() {}
SendDirectMessageHandler::SendDirectMessageHandler() {}

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