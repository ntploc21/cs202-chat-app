#include "Message.hpp"

Message::Message() {}

Message::Message(int msg_id, int from_id, int to_conversation_id,
                 std::string content, MessageType type, bool forward,
                 bool active)
    : m_msg_id(msg_id), m_from_id(from_id),
      m_to_conversation_id(to_conversation_id), m_content(content),
      m_type(type), m_forward(forward), m_active(active) {}

int Message::get_msg_id() const { return m_msg_id; }

int Message::get_from_id() const { return m_from_id; }

int Message::get_to_conversation_id() const { return m_to_conversation_id; }

std::string Message::get_content() const { return m_content; }

MessageType Message::get_type() const { return m_type; }

bool Message::is_forwarded() const { return m_forward; }

bool Message::is_active() const { return m_active; }

Message& Message::set_msg_id(int msg_id) {
    m_msg_id = msg_id;
    return *this;
}

Message& Message::set_from_id(int from_id) {
    m_from_id = from_id;
    return *this;
}

Message& Message::set_to_conversation_id(int to_conversation_id) {
    m_to_conversation_id = to_conversation_id;
    return *this;
}

Message& Message::set_content(std::string content) {
    m_content = content;
    return *this;
}

Message& Message::set_type(MessageType type) {
    m_type = type;
    return *this;
}

Message& Message::set_forward(bool forward) {
    m_forward = forward;
    return *this;
}

Message& Message::set_active(bool active) {
    m_active = active;
    return *this;
}


std::ostream& operator<<(std::ostream& os, const Message& msg) {
    os << "Message ID: " << msg.m_msg_id << std::endl
       << "From ID: " << msg.m_from_id << std::endl
       << "To Conversation ID: " << msg.m_to_conversation_id << std::endl
       << "Content: " << msg.m_content << std::endl
       << "Type: " << MessageTypeToString(msg.m_type) << std::endl
       << "Forwarded: " << msg.m_forward << std::endl
       << "Active: " << msg.m_active << std::endl;
    return os;
}

void Message::Serialize(Walnut::StreamWriter* serializer, const Message& instance) {
    serializer->WriteRaw< int >(instance.m_msg_id);
    serializer->WriteRaw< int >(instance.m_from_id);
    serializer->WriteRaw< int >(instance.m_to_conversation_id);
    serializer->WriteString(instance.m_content);
    serializer->WriteRaw< MessageType >(instance.m_type);
    serializer->WriteRaw< bool >(instance.m_forward);
    serializer->WriteRaw< bool >(instance.m_active);
}

void Message::Deserialize(Walnut::StreamReader* deserializer,
                          Message& instance) {
    deserializer->ReadRaw< int >(instance.m_msg_id);
    deserializer->ReadRaw< int >(instance.m_from_id);
    deserializer->ReadRaw< int >(instance.m_to_conversation_id);
    deserializer->ReadString(instance.m_content);
	deserializer->ReadRaw< MessageType >(instance.m_type);
	deserializer->ReadRaw< bool >(instance.m_forward);
	deserializer->ReadRaw< bool >(instance.m_active);
}

std::string_view MessageTypeToString(MessageType type) {
    switch (type) {
        case MessageType::Text:
            return "TEXT";
        case MessageType::Image:
            return "IMAGE";
        case MessageType::File:
            return "FILE";
        case MessageType::Announcement:
            return "ANNOUNCEMENT";
        default:
            return "INVALID";
    }
}