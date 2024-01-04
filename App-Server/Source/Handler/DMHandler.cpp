#include "DMHandler.hpp"

#include <iostream>

#include "../ClientManager.hpp"
#include "ConversationManager.hpp"
#include "DirectMessageManager.hpp"
#include "Walnut/Serialization/BufferStream.h"

RetrieveAllDMsHandler::RetrieveAllDMsHandler() {}

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
    out_stream.WriteRaw<bool>(true);
    out_stream.WriteArray(dms);
    out_stream.WriteArray(conversations);
    out_stream.WriteArray(messages);

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}
