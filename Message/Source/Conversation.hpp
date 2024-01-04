#pragma once

#include <vector>

#include "Message.hpp"
#include "Walnut/Serialization/StreamReader.h"
#include "Walnut/Serialization/StreamWriter.h"

class Conversation {
public:
    Conversation();
    virtual ~Conversation() = default;

    Conversation& set_conversation_id(int conversation_id);
    int get_conversation_id() const;

    std::vector< int > get_messages_id() const;
    std::vector< Message > get_messages() const;
    std::optional< Message > get_last_message() const;

    Conversation& add_message(int message_id);
    Conversation& add_message(Message message);

    std::optional< Message > send_message(int sender_id, std::string content);

    Conversation& remove_message(int message_id);
    Conversation& remove_message(Message message);

    Conversation& clear_messages();

    friend std::ostream& operator<<(std::ostream& out,
                                    const Conversation& conversation);

    static void Serialize(Walnut::StreamWriter* serializer,
                          const Conversation& instance);

    static void Deserialize(Walnut::StreamReader* deserializer,
                            Conversation& instance);

    friend YAML::Emitter& operator<<(YAML::Emitter& out,
                                     const Conversation& conversation);

    friend void operator>>(const YAML::Node& in, Conversation& conversation);

private:
    int m_conversation_id{};
    std::vector< int > m_messages{};  // contains message ids
    date::sys_seconds m_last_msg_at{};
};