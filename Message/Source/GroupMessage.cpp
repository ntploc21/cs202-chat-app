#include "GroupMessage.hpp"
#include "ConversationManager.hpp"

GroupMessage::GroupMessage() {}

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

void GroupMessage::Serialize(Walnut::StreamWriter* serializer,
                             const GroupMessage& instance) {
    serializer->WriteRaw< int >(instance.m_group_id);
    serializer->WriteRaw< int >(instance.m_conversation_id);
    serializer->WriteString(instance.m_group_name);
    serializer->WriteArray(instance.m_member_list);
    serializer->WriteArray(instance.m_admin_list);
    serializer->WriteMap(instance.m_nickname_list);
    serializer->WriteArray(instance.m_pin_message_list);
}

void GroupMessage::Deserialize(Walnut::StreamReader* deserializer,
                               GroupMessage& instance) {
    deserializer->ReadRaw< int >(instance.m_group_id);
    deserializer->ReadRaw< int >(instance.m_conversation_id);
    deserializer->ReadString(instance.m_group_name);
    deserializer->ReadArray(instance.m_member_list);
    deserializer->ReadArray(instance.m_admin_list);
    deserializer->ReadMap(instance.m_nickname_list);
    deserializer->ReadArray(instance.m_pin_message_list);
}

std::optional< Message > GroupMessage::send_message(int sender_id,
    													Message message) const {
	if (!is_member(sender_id)) return std::nullopt;
    auto msg = ConversationManager::getInstance().send_message(sender_id, m_conversation_id, message);

    return msg;

}

std::optional< Message > GroupMessage::send_message(int sender_id,
    														std::string message) const {
	if (!is_member(sender_id)) return std::nullopt;
	auto msg = ConversationManager::getInstance().send_message(sender_id, m_conversation_id, message);

	return msg;
}