#include "DirectMessage.hpp"

#include "ConversationManager.hpp"
#include "UserManager.hpp"

DirectMessage::DirectMessage() {}

DirectMessage::DirectMessage(int dm_id, int conversation_id, int user_id_1,
                             int user_id_2, std::string user_1_nickname,
                             std::string user_2_nickname) {
    m_dm_id = dm_id;
    m_conversation_id = conversation_id;
    m_user_id_1 = user_id_1;
    m_user_id_2 = user_id_2;

    if (user_1_nickname == "") {
        user_1_nickname = UserManager::getInstance()
                              .get_user(user_id_1)
                              .value()
                              .get_username();
    }

    if (user_2_nickname == "") {
        user_2_nickname = UserManager::getInstance()
                              .get_user(user_id_2)
                              .value()
                              .get_username();
    }

    m_user_1_nickname = user_1_nickname;
    m_user_2_nickname = user_2_nickname;
}

DirectMessage& DirectMessage::set_dm_id(int dm_id) {
    m_dm_id = dm_id;
    return *this;
}

int DirectMessage::get_dm_id() const { return m_dm_id; }

DirectMessage& DirectMessage::set_conversation_id(int conversation_id) {
    m_conversation_id = conversation_id;
    return *this;
}

int DirectMessage::get_conversation_id() const { return m_conversation_id; }

DirectMessage& DirectMessage::set_user_id_1(int user_id_1) {
    m_user_id_1 = user_id_1;
    return *this;
}

int DirectMessage::get_user_id_1() const { return m_user_id_1; }

DirectMessage& DirectMessage::set_user_id_2(int user_id_2) {
    m_user_id_2 = user_id_2;
    return *this;
}

int DirectMessage::get_user_id_2() const { return m_user_id_2; }

DirectMessage& DirectMessage::set_user_nickname(int user_id,
                                                std::string nickname) {
    if (user_id == m_user_id_1) {
        m_user_1_nickname = nickname;
    } else if (user_id == m_user_id_2) {
        m_user_2_nickname = nickname;
    } else {
        throw std::invalid_argument("User id is not in this conversation");
    }

    return *this;
}

std::string DirectMessage::get_user_nickname(int user_id) const {
    if (user_id == m_user_id_1) {
        return m_user_1_nickname;
    } else if (user_id == m_user_id_2) {
        return m_user_2_nickname;
    } else {
        throw std::invalid_argument("User id is not in this conversation");
    }
}

std::vector< int > DirectMessage::get_user_list_ids() const {
    return {m_user_id_1, m_user_id_2};
}

DirectMessage& DirectMessage::add_pin_message(int message_id) {
    m_pin_message_list.push_back(message_id);
    return *this;
}

DirectMessage& DirectMessage::remove_pin_message(int message_id) {
    auto it = std::find(m_pin_message_list.begin(), m_pin_message_list.end(),
                        message_id);
    if (it != m_pin_message_list.end()) {
        m_pin_message_list.erase(it);
    }
    return *this;
}

std::vector< int > DirectMessage::get_pin_message_list_ids() const {
    return m_pin_message_list;
}

bool DirectMessage::is_pin_message(int message_id) const {
    return std::find(m_pin_message_list.begin(), m_pin_message_list.end(),
                     message_id) != m_pin_message_list.end();
}

void DirectMessage::set_user_last_seen_at(int user_id,
                                          date::sys_seconds last_seen_at) {
    if (user_id == m_user_id_1) {
        m_user_1_last_seen_at = last_seen_at;
    } else if (user_id == m_user_id_2) {
        m_user_2_last_seen_at = last_seen_at;
    } else {
        throw std::invalid_argument("User id is not in this conversation");
    }
}

date::sys_seconds DirectMessage::get_user_last_seen_at(int user_id) const {
    if (user_id == m_user_id_1) {
        return m_user_1_last_seen_at;
    } else if (user_id == m_user_id_2) {
        return m_user_2_last_seen_at;
    }
    throw std::invalid_argument("User id is not in this conversation");
}

std::ostream& operator<<(std::ostream& out,
                         const DirectMessage& direct_message) {
    out << "DirectMessage {";
    out << "\n\tdm_id: " << direct_message.m_dm_id;
    out << "\n\tconversation_id: " << direct_message.m_conversation_id;
    out << "\n\tuser_1: " << direct_message.m_user_id_1 << " ("
        << direct_message.m_user_1_nickname << ")";
    out << "\n\tuser_2: " << direct_message.m_user_id_2 << " ("
        << direct_message.m_user_2_nickname << ")";
    out << "\n\tm_pin_message_list: [";
    for (auto it = direct_message.m_pin_message_list.begin();
         it != direct_message.m_pin_message_list.end(); ++it) {
        out << *it;
        if (it != direct_message.m_pin_message_list.end() - 1) {
            out << ", ";
        }
    }
    out << "]\n";

    out << "\n\tm_user_1_last_seen_at: "
        << direct_message.m_user_1_last_seen_at;

    out << "\n\tm_user_2_last_seen_at: "
        << direct_message.m_user_2_last_seen_at;

    out << "}";
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out,
                          const DirectMessage& direct_message) {
    out << YAML::BeginMap;

    out << YAML::Key << "dm_id" << YAML::Value << direct_message.m_dm_id;

    out << YAML::Key << "conversation_id" << YAML::Value
        << direct_message.m_conversation_id;

    out << YAML::Key << "user_id_1" << YAML::Value
        << direct_message.m_user_id_1;

    out << YAML::Key << "user_id_2" << YAML::Value
        << direct_message.m_user_id_2;

    out << YAML::Key << "user_1_nickname" << YAML::Value
        << direct_message.m_user_1_nickname;

    out << YAML::Key << "user_2_nickname" << YAML::Value
        << direct_message.m_user_2_nickname;

    out << YAML::Key << "pin_message_list" << YAML::Value
        << direct_message.m_pin_message_list;

    out << YAML::Key << "user_1_last_seen_at" << YAML::Value
        << direct_message.m_user_1_last_seen_at.time_since_epoch().count();

    out << YAML::Key << "user_2_last_seen_at" << YAML::Value
        << direct_message.m_user_2_last_seen_at.time_since_epoch().count();

    out << YAML::EndMap;

    return out;
}

void operator>>(const YAML::Node& in, DirectMessage& direct_message) {
    direct_message.m_dm_id = in["dm_id"].as< int >();
    direct_message.m_conversation_id = in["conversation_id"].as< int >();
    direct_message.m_user_id_1 = in["user_id_1"].as< int >();
    direct_message.m_user_id_2 = in["user_id_2"].as< int >();
    direct_message.m_user_1_nickname =
        in["user_1_nickname"].as< std::string >();
    direct_message.m_user_2_nickname =
        in["user_2_nickname"].as< std::string >();

    direct_message.m_pin_message_list =
        in["pin_message_list"].as< std::vector< int > >();

    direct_message.m_user_1_last_seen_at =
        date::sys_seconds(date::days(in["user_1_last_seen_at"].as< int >()));

    direct_message.m_user_2_last_seen_at =
        date::sys_seconds(date::days(in["user_2_last_seen_at"].as< int >()));
}

void DirectMessage::Serialize(Walnut::StreamWriter* serializer,
                              const DirectMessage& instance) {
    serializer->WriteRaw< int >(instance.m_dm_id);
    serializer->WriteRaw< int >(instance.m_conversation_id);
    serializer->WriteRaw< int >(instance.m_user_id_1);
    serializer->WriteRaw< int >(instance.m_user_id_2);
    serializer->WriteString(instance.m_user_1_nickname);
    serializer->WriteString(instance.m_user_2_nickname);
    serializer->WriteArray(instance.m_pin_message_list);
    serializer->WriteRaw< int >(
        instance.m_user_1_last_seen_at.time_since_epoch().count());
    serializer->WriteRaw< int >(
        instance.m_user_2_last_seen_at.time_since_epoch().count());
}

void DirectMessage::Deserialize(Walnut::StreamReader* deserializer,
                                DirectMessage& instance) {
    deserializer->ReadRaw< int >(instance.m_dm_id);
    deserializer->ReadRaw< int >(instance.m_conversation_id);
    deserializer->ReadRaw< int >(instance.m_user_id_1);
    deserializer->ReadRaw< int >(instance.m_user_id_2);
    deserializer->ReadString(instance.m_user_1_nickname);
    deserializer->ReadString(instance.m_user_2_nickname);
    deserializer->ReadArray(instance.m_pin_message_list);

    int user_1_last_seen_at;
    deserializer->ReadRaw< int >(user_1_last_seen_at);
    instance.m_user_1_last_seen_at =
        date::sys_seconds(date::days(user_1_last_seen_at));

    int user_2_last_seen_at;
    deserializer->ReadRaw< int >(user_2_last_seen_at);
    instance.m_user_2_last_seen_at =
        date::sys_seconds(date::days(user_2_last_seen_at));
}

std::optional< Message > DirectMessage::send_message(int sender_id,
                                                     Message message) const {
    if (sender_id != m_user_id_1 && sender_id != m_user_id_2) {
        return std::nullopt;
    }

    return ConversationManager::getInstance().send_message(
        sender_id, m_conversation_id, message);
}

std::optional< Message > DirectMessage::send_message(
    int sender_id, std::string message) const {
    if (sender_id != m_user_id_1 && sender_id != m_user_id_2) {
        return std::nullopt;
    }

    return ConversationManager::getInstance().send_message(
        sender_id, m_conversation_id, message);
}

std::optional< Message > DirectMessage::send_announcement(
    int sender_id, std::string message) const {
    if (sender_id != m_user_id_1 && sender_id != m_user_id_2) {
        return std::nullopt;
    }

    return ConversationManager::getInstance().send_announcement(
        sender_id, m_conversation_id, message);
}
