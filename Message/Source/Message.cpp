#include "Message.hpp"

Message::Message() {
    m_created_at = date::floor< date::days >(std::chrono::system_clock::now());
}

Message::Message(int msg_id, int from_id, int to_conversation_id,
                 std::string content, MessageType type, bool forward,
                 bool active)
    : m_msg_id(msg_id), m_from_id(from_id),
      m_to_conversation_id(to_conversation_id), m_content(content),
      m_type(type), m_forward(forward), m_active(active) {
    m_created_at = date::floor< date::days >(std::chrono::system_clock::now());
}

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
        << "Created At: " << msg.m_created_at << std::endl
       << "Type: " << MessageTypeToString(msg.m_type) << std::endl
       << "Forwarded: " << msg.m_forward << std::endl
       << "Active: " << msg.m_active << std::endl;
    return os;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Message& user) {
    out << YAML::BeginMap;

    out << YAML::Key << "msg_id" << YAML::Value << user.m_msg_id;

    out << YAML::Key << "from_id" << YAML::Value << user.m_from_id;

    out << YAML::Key << "to_conversation_id" << YAML::Value
        << user.m_to_conversation_id;

    out << YAML::Key << "content" << YAML::Value << user.m_content;

    out << YAML::Key << "created_at" << YAML::Value << user.m_created_at.time_since_epoch().count();
        
    out << YAML::Key << "type" << YAML::Value
        << MessageTypeToString(user.m_type).data();

    out << YAML::Key << "forward" << YAML::Value << user.m_forward;

    out << YAML::Key << "active" << YAML::Value << user.m_active;

    out << YAML::EndMap;

    return out;
}

void operator>>(const YAML::Node& in, Message& user) {
    user.m_msg_id = in["msg_id"].as< int >();
	user.m_from_id = in["from_id"].as< int >();
	user.m_to_conversation_id = in["to_conversation_id"].as< int >();
	user.m_content = in["content"].as< std::string >();
    user.m_created_at = date::sys_seconds(date::days(in["created_at"].as< int >()));
	user.m_type = StringToMessageType(in["type"].as< std::string >());
	user.m_forward = in["forward"].as< bool >();
	user.m_active = in["active"].as< bool >();
}

void Message::Serialize(Walnut::StreamWriter* serializer,
                        const Message& instance) {
    serializer->WriteRaw< int >(instance.m_msg_id);
    serializer->WriteRaw< int >(instance.m_from_id);
    serializer->WriteRaw< int >(instance.m_to_conversation_id);
    serializer->WriteString(instance.m_content);
    serializer->WriteRaw< int >(instance.m_created_at.time_since_epoch().count());
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

    int created_at;
    deserializer->ReadRaw< int >(created_at);
    instance.m_created_at = date::sys_seconds(date::days(created_at));

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

MessageType StringToMessageType(std::string_view type) {
    if (type == "TEXT") {
        return MessageType::Text;
    } else if (type == "IMAGE") {
        return MessageType::Image;
    } else if (type == "FILE") {
        return MessageType::File;
    } else if (type == "ANNOUNCEMENT") {
        return MessageType::Announcement;
    } else {
        return MessageType::None;
    }
}