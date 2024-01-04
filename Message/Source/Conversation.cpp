#include "Conversation.hpp"

#include <iostream>

#include "MessageManager.hpp"

Conversation::Conversation() {
    m_last_msg_at = date::floor< date::days >(std::chrono::system_clock::now());
}

Conversation& Conversation::set_conversation_id(int conversation_id) {
    m_conversation_id = conversation_id;
    return *this;
}

int Conversation::get_conversation_id() const { return m_conversation_id; }

std::vector< int > Conversation::get_messages_id() const { return m_messages; }

std::vector< Message > Conversation::get_messages() const {
    std::vector< Message > messages{};

    for (int i = 0; i < m_messages.size(); i++) {
        auto msg =
            MessageManager::getInstance().get_message(m_messages[i]);
        assert(msg.has_value());
        messages.push_back(msg.value());
    }
    return messages;
}

std::optional< Message > Conversation::get_last_message() const {
    if (m_messages.size() == 0) return std::nullopt;

    return MessageManager::getInstance().get_message(m_messages.back());
}

Conversation& Conversation::add_message(int message_id) {
    m_last_msg_at = date::floor< date::days >(std::chrono::system_clock::now());
    m_messages.push_back(message_id);
    return *this;
}

Conversation& Conversation::add_message(Message message) {
    m_last_msg_at = date::floor< date::days >(std::chrono::system_clock::now());
    m_messages.push_back(message.get_msg_id());
    return *this;
}

std::optional< Message > Conversation::send_message(int sender_id,
                                                    std::string content) {
    auto msg = MessageManager::getInstance().send_message(
        sender_id, m_conversation_id, content);

    if (!msg.has_value()) return std::nullopt;
    m_last_msg_at = date::floor< date::days >(std::chrono::system_clock::now());

    m_messages.push_back(msg.value().get_msg_id());
    return msg;
}

Conversation& Conversation::remove_message(int message_id) {
    for (int i = 0; i < m_messages.size(); i++) {
        if (m_messages[i] == message_id) {
            m_messages.erase(m_messages.begin() + i);
            return *this;
        }
    }
    return *this;
}

Conversation& Conversation::remove_message(Message message) {
    for (int i = 0; i < m_messages.size(); i++) {
        if (m_messages[i] == message.get_msg_id()) {
            m_messages.erase(m_messages.begin() + i);
            return *this;
        }
    }
    return *this;
}

Conversation& Conversation::clear_messages() {
    m_messages.clear();
    return *this;
}

std::ostream& operator<<(std::ostream& out, const Conversation& conversation) {
    out << "Conversation: " << conversation.m_conversation_id << std::endl;
    out << "Messages: ";
    for (int i = 0; i < conversation.m_messages.size(); i++) {
        out << conversation.m_messages[i] << " ";
    }
    out << std::endl;
    out << "Last Message At: " << conversation.m_last_msg_at << std::endl;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out,
                          const Conversation& conversation) {
    out << YAML::BeginMap;
    out << YAML::Key << "conversation_id" << YAML::Value
        << conversation.m_conversation_id;

    out << YAML::Key << "messages" << YAML::Value << YAML::BeginSeq;
    for (int i = 0; i < conversation.m_messages.size(); i++) {
        out << conversation.m_messages[i];
    }
    out << YAML::EndSeq;

    out << YAML::Key << "last_msg_at" << YAML::Value
        << conversation.m_last_msg_at.time_since_epoch().count();

    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, Conversation& conversation) {
    conversation.m_conversation_id = in["conversation_id"].as< int >();

    for (int i = 0; i < in["messages"].size(); i++) {
        conversation.m_messages.push_back(in["messages"][i].as< int >());
    }

    conversation.m_last_msg_at =
        date::sys_seconds(std::chrono::seconds(in["last_msg_at"].as< int >()));
}

void Conversation::Serialize(Walnut::StreamWriter* serializer,
                             const Conversation& instance) {
    serializer->WriteRaw< int >(instance.m_conversation_id);
    serializer->WriteArray(instance.m_messages);
    serializer->WriteRaw< int >(
        instance.m_last_msg_at.time_since_epoch().count());
}

void Conversation::Deserialize(Walnut::StreamReader* deserializer,
                               Conversation& instance) {
    deserializer->ReadRaw< int >(instance.m_conversation_id);
    deserializer->ReadArray(instance.m_messages);

    int last_msg_at;
    deserializer->ReadRaw< int >(last_msg_at);
    instance.m_last_msg_at = date::floor< date::days >(
        std::chrono::system_clock::now() + std::chrono::seconds(last_msg_at));
}