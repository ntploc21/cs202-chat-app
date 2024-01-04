#include "GroupHandler.hpp"

#include "../ClientManager.hpp"
#include "ConversationManager.hpp"
#include "GroupMessageManager.hpp"
#include "Walnut/Serialization/BufferStream.h"

RetrieveAllGroupsHandler::RetrieveAllGroupsHandler() {}

void RetrieveAllGroupsHandler::handleImpl(const Walnut::ClientInfo& client_info,
                                          const PacketType packet_type,
                                          Walnut::BufferStreamReader& stream) {
    auto& client = ClientManager::getInstance()[client_info.ID];

    std::vector< GroupMessage > groups =
        GroupMessageManager::getInstance().get_group_messages();

    std::vector< Conversation > conversations;

    for (auto& group : groups) {
        auto tmp = ConversationManager::getInstance().get_conversation(
            group.get_conversation_id());

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
    out_stream.WriteRaw< PacketType >(PacketType::RetrieveAllGroups);
    out_stream.WriteRaw< bool >(true);
    out_stream.WriteArray(groups);
    out_stream.WriteArray(conversations);
    out_stream.WriteArray(messages);

    m_server->SendBufferToClient(
        client_info.ID,
        Walnut::Buffer(scratch_buffer, out_stream.GetStreamPosition()));
}
