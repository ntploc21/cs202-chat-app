#include "GroupMessage.hpp"

#include "ConversationManager.hpp"

GroupMessage::GroupMessage() {}

GroupMessage::GroupMessage(int group_id, int conversation_id,
                           std::string group_name,
                           std::vector< int > member_list) {
    m_group_id = group_id;
    m_conversation_id = conversation_id;
    m_group_name = group_name;
    m_member_list = member_list;

    assert(m_member_list.size() > 0);
    m_admin_list.push_back(m_member_list[0]);
}

GroupMessage& GroupMessage::set_group_id(int group_id) {
    m_group_id = group_id;
    return *this;
}

int GroupMessage::get_group_id() const { return m_group_id; }

GroupMessage& GroupMessage::set_conversation_id(int conversation_id) {
    m_conversation_id = conversation_id;
    return *this;
}

int GroupMessage::get_conversation_id() const { return m_conversation_id; }

GroupMessage& GroupMessage::set_group_name(std::string group_name) {
    m_group_name = group_name;
    return *this;
}

std::string GroupMessage::get_group_name() const { return m_group_name; }

GroupMessage& GroupMessage::add_member(int member_id) {
    if (is_member(member_id)) return *this;
    m_member_list.push_back(member_id);
    return *this;
}

GroupMessage& GroupMessage::remove_member(int member_id) {
    int pos = -1;

    for (int i = 0; i < m_member_list.size(); i++) {
        if (m_member_list[i] == member_id) {
            pos = i;
            break;
        }
    }

    if (pos != -1) {
        m_member_list.erase(m_member_list.begin() + pos);
    }

    return *this;
}

bool GroupMessage::is_member(int member_id) const {
    for (int i = 0; i < m_member_list.size(); i++) {
        if (m_member_list[i] == member_id) {
            return true;
        }
    }
    return false;
}

std::vector< int > GroupMessage::get_member_list_ids() const {
    return m_member_list;
}

GroupMessage& GroupMessage::add_admin(int admin_id) {
    if (is_admin(admin_id)) return *this;
    m_admin_list.push_back(admin_id);
    return *this;
}

GroupMessage& GroupMessage::remove_admin(int admin_id) {
    int pos = -1;

    for (int i = 0; i < m_admin_list.size(); i++) {
        if (m_admin_list[i] == admin_id) {
            pos = i;
            break;
        }
    }

    if (pos != -1) {
        m_admin_list.erase(m_admin_list.begin() + pos);
    }

    return *this;
}

bool GroupMessage::is_admin(int admin_id) const {
    for (int i = 0; i < m_admin_list.size(); i++) {
        if (m_admin_list[i] == admin_id) {
            return true;
        }
    }
    return false;
}

std::vector< int > GroupMessage::get_admin_list_ids() const {
    return m_admin_list;
}

GroupMessage& GroupMessage::set_nickname(int user_id, std::string nickname) {
    if (!is_member(user_id)) return *this;
    m_nickname_list[user_id] = nickname;
    return *this;
}

std::string GroupMessage::get_nickname(int user_id) const {
    return m_nickname_list.at(user_id);
}

std::map< int, std::string > GroupMessage::get_nickname_list() const {
    return m_nickname_list;
}

GroupMessage& GroupMessage::add_pin_message(int message_id) {
    if (is_pin_message(message_id)) return *this;
    m_pin_message_list.push_back(message_id);
    return *this;
}

GroupMessage& GroupMessage::remove_pin_message(int message_id) {
    int pos = -1;

    for (int i = 0; i < m_pin_message_list.size(); i++) {
        if (m_pin_message_list[i] == message_id) {
            pos = i;
            break;
        }
    }

    if (pos != -1) {
        m_pin_message_list.erase(m_pin_message_list.begin() + pos);
    }

    return *this;
}

bool GroupMessage::is_pin_message(int message_id) const {
    for (int i = 0; i < m_pin_message_list.size(); i++) {
        if (m_pin_message_list[i] == message_id) {
            return true;
        }
    }
    return false;
}

std::vector< int > GroupMessage::get_pin_message_list_ids() const {
    return m_pin_message_list;
}

std::ostream& operator<<(std::ostream& out, const GroupMessage& group_message) {
    out << "GroupMessage{\n"
        << "group_id=" << group_message.m_group_id << ",\n"
        << "conversation_id=" << group_message.m_conversation_id << ",\n"
        << "group_name=" << group_message.m_group_name << ",\n"
        << "member_list=[";

    for (int i = 0; i < group_message.m_member_list.size(); i++) {
        out << group_message.m_member_list[i];
        if (i != group_message.m_member_list.size() - 1) {
            out << ", ";
        }
    }
    out << "],\n";

    out << "admin_list=[";
    for (int i = 0; i < group_message.m_admin_list.size(); i++) {
        out << group_message.m_admin_list[i];
        if (i != group_message.m_admin_list.size() - 1) {
            out << ", ";
        }
    }

    out << "],\n"
        << "nickname_list=[";
    for (auto& [key, value] : group_message.m_nickname_list) {
        out << key << ": " << value << ", ";
    }

    out << "],\n"
        << "pin_message_list=[";
    for (int i = 0; i < group_message.m_pin_message_list.size(); i++) {
        out << group_message.m_pin_message_list[i];
        if (i != group_message.m_pin_message_list.size() - 1) {
            out << ", ";
        }
    }

    out << "]\n"
        << "}";

    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out,
                          const GroupMessage& group_message) {
    out << YAML::BeginMap;

    out << YAML::Key << "group_id" << YAML::Value << group_message.m_group_id;

    out << YAML::Key << "conversation_id" << YAML::Value
        << group_message.m_conversation_id;

    out << YAML::Key << "group_name" << YAML::Value
        << group_message.m_group_name;

    out << YAML::Key << "member_list" << YAML::Value << YAML::BeginSeq;
    for (int i = 0; i < group_message.m_member_list.size(); i++) {
        out << group_message.m_member_list[i];
    }
    out << YAML::EndSeq;

    out << YAML::Key << "admin_list" << YAML::Value << YAML::BeginSeq;
    for (int i = 0; i < group_message.m_admin_list.size(); i++) {
        out << group_message.m_admin_list[i];
    }
    out << YAML::EndSeq;

    out << YAML::Key << "nickname_list" << YAML::Value << YAML::BeginSeq;
    for (auto& [key, value] : group_message.m_nickname_list) {
        out << YAML::BeginSeq << key << value << YAML::EndSeq;
    }
    out << YAML::EndMap;

    out << YAML::Key << "pin_message_list" << YAML::Value << YAML::BeginSeq;
    for (int i = 0; i < group_message.m_pin_message_list.size(); i++) {
        out << group_message.m_pin_message_list[i];
    }
    out << YAML::EndSeq;

    out << YAML::EndMap;

    return out;
}

void operator>>(const YAML::Node& in, GroupMessage& group_message) {
    group_message.m_group_id = in["group_id"].as< int >();
    group_message.m_conversation_id = in["conversation_id"].as< int >();
    group_message.m_group_name = in["group_name"].as< std::string >();

    for (int i = 0; i < in["member_list"].size(); i++) {
        group_message.m_member_list.push_back(in["member_list"][i].as< int >());
    }

    for (int i = 0; i < in["admin_list"].size(); i++) {
        group_message.m_admin_list.push_back(in["admin_list"][i].as< int >());
    }

    for (int i = 0; i < in["nickname_list"].size(); i++) {
        group_message.m_nickname_list[in["nickname_list"][i][0].as< int >()] =
            in["nickname_list"][i][1].as< std::string >();
    }

    group_message.m_pin_message_list.clear();
    for (int i = 0; i < in["pin_message_list"].size(); i++) {
        group_message.m_pin_message_list.push_back(
            in["pin_message_list"][i].as< int >());
    }
}

void GroupMessage::Serialize(Walnut::StreamWriter* serializer,
                             const GroupMessage& instance) {
    serializer->WriteRaw< int >(instance.m_group_id);
    serializer->WriteRaw< int >(instance.m_conversation_id);
    serializer->WriteString(instance.m_group_name);
    serializer->WriteArray(instance.m_member_list);
    serializer->WriteArray(instance.m_admin_list);

    serializer->WriteRaw< uint32_t >(instance.m_nickname_list.size());

    for (auto& [key, value] : instance.m_nickname_list) {
        serializer->WriteRaw< int >(key);
        serializer->WriteString(value);
    }

    serializer->WriteArray(instance.m_pin_message_list);
}

void GroupMessage::Deserialize(Walnut::StreamReader* deserializer,
                               GroupMessage& instance) {
    deserializer->ReadRaw< int >(instance.m_group_id);
    deserializer->ReadRaw< int >(instance.m_conversation_id);
    deserializer->ReadString(instance.m_group_name);
    deserializer->ReadArray(instance.m_member_list);
    deserializer->ReadArray(instance.m_admin_list);

    uint32_t size;
    deserializer->ReadRaw< uint32_t >(size);

    for (int i = 0; i < size; i++) {
        int key;
        std::string value;
        deserializer->ReadRaw< int >(key);
        deserializer->ReadString(value);
        instance.m_nickname_list[key] = value;
    }

    deserializer->ReadArray(instance.m_pin_message_list);
}

std::optional< Message > GroupMessage::send_message(int sender_id,
                                                    Message message) const {
    if (!is_member(sender_id)) return std::nullopt;
    auto msg = ConversationManager::getInstance().send_message(
        sender_id, m_conversation_id, message);

    return msg;
}

std::optional< Message > GroupMessage::send_message(int sender_id,
                                                    std::string message) const {
    if (!is_member(sender_id)) return std::nullopt;
    auto msg = ConversationManager::getInstance().send_message(
        sender_id, m_conversation_id, message);

    return msg;
}