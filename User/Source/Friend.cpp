#include "Friend.hpp"

Friend::Friend() {
    m_created_at = date::floor< date::days >(std::chrono::system_clock::now());
}

Friend::Friend(int friend_id, int user_1_id, int user_2_id, int user_1_status,
               int user_2_status, std::string note, int conversation_id)
    : m_friend_id(friend_id), m_user_1_id(user_1_id), m_user_2_id(user_2_id),
      m_user_1_status(user_1_status), m_user_2_status(user_2_status),
      m_note(note), m_conversation_id(conversation_id) {
    m_created_at = date::floor< date::days >(std::chrono::system_clock::now());
}

int Friend::get_friend_id() const { return m_friend_id; }

int Friend::get_user_1_id() const { return m_user_1_id; }

int Friend::get_user_2_id() const { return m_user_2_id; }

int Friend::get_user_1_status() const { return m_user_1_status; }

int Friend::get_user_2_status() const { return m_user_2_status; }

std::string Friend::get_note() const { return m_note; }

int Friend::get_conversation_id() const { return m_conversation_id; }

date::sys_seconds Friend::get_created_at() const { return m_created_at; }

Friend &Friend::set_friend_id(int friend_id) {
    m_friend_id = friend_id;
    return *this;
}

Friend &Friend::set_user_1_id(int user_1_id) {
    m_user_1_id = user_1_id;
    return *this;
}

Friend &Friend::set_user_2_id(int user_2_id) {
    m_user_2_id = user_2_id;
    return *this;
}

Friend &Friend::set_user_1_status(int user_1_status) {
    m_user_1_status = user_1_status;
    return *this;
}

Friend &Friend::set_user_2_status(int user_2_status) {
    m_user_2_status = user_2_status;
    return *this;
}

Friend &Friend::set_note(std::string note) {
    m_note = note;
    return *this;
}

Friend &Friend::set_conversation_id(int conversation_id) {
    m_conversation_id = conversation_id;
    return *this;
}

Friend &Friend::set_created_at(date::sys_seconds created_at) {
    m_created_at = created_at;
    return *this;
}

std::ostream &operator<<(std::ostream &out, const Friend &friend_) {
    out << "Friend id: " << friend_.m_friend_id << "\n"
        << "User 1 id: " << friend_.m_user_1_id << "\n"
        << "User 2 id: " << friend_.m_user_2_id << "\n"
        << "User 1 status: " << friend_.m_user_1_status << "\n"
        << "User 2 status: " << friend_.m_user_2_status << "\n"
        << "Note: " << friend_.m_note << "\n"
        << "Conversation id: " << friend_.m_conversation_id << "\n"
        << "Created at: " << friend_.m_created_at << "\n";

    return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Friend &friend_) {
    out << YAML::BeginMap;

    out << YAML::Key << "friend_id" << YAML::Value << friend_.m_friend_id;

    out << YAML::Key << "user_1_id" << YAML::Value << friend_.m_user_1_id;

    out << YAML::Key << "user_2_id" << YAML::Value << friend_.m_user_2_id;

    out << YAML::Key << "user_1_status" << YAML::Value
        << friend_.m_user_1_status;

    out << YAML::Key << "user_2_status" << YAML::Value
        << friend_.m_user_2_status;

    out << YAML::Key << "note" << YAML::Value << friend_.m_note;

    out << YAML::Key << "conversation_id" << YAML::Value
        << friend_.m_conversation_id;

    out << YAML::Key << "created_at" << YAML::Value
        << friend_.m_created_at.time_since_epoch().count();

    out << YAML::EndMap;

    return out;
}

void operator>>(const YAML::Node &in, Friend &friend_) {
    friend_.m_friend_id = in["friend_id"].as< int >();
    friend_.m_user_1_id = in["user_1_id"].as< int >();
    friend_.m_user_2_id = in["user_2_id"].as< int >();
    friend_.m_user_1_status = in["user_1_status"].as< int >();
    friend_.m_user_2_status = in["user_2_status"].as< int >();
    friend_.m_note = in["note"].as< std::string >();
    friend_.m_conversation_id = in["conversation_id"].as< int >();
    friend_.m_created_at =
        date::sys_seconds(std::chrono::seconds(in["created_at"].as< int >()));
}

void Friend::Serialize(Walnut::StreamWriter *serializer,
                       const Friend &instance) {
    serializer->WriteRaw< int >(instance.m_friend_id);
    serializer->WriteRaw< int >(instance.m_user_1_id);
    serializer->WriteRaw< int >(instance.m_user_2_id);
    serializer->WriteRaw< int >(instance.m_user_1_status);
    serializer->WriteRaw< int >(instance.m_user_2_status);
    serializer->WriteString(instance.m_note);
    serializer->WriteRaw< int >(instance.m_conversation_id);
    serializer->WriteRaw< int >(
        instance.m_created_at.time_since_epoch().count());
}

void Friend::Deserialize(Walnut::StreamReader *deserializer, Friend &instance) {
    deserializer->ReadRaw< int >(instance.m_friend_id);
    deserializer->ReadRaw< int >(instance.m_user_1_id);
    deserializer->ReadRaw< int >(instance.m_user_2_id);
    deserializer->ReadRaw< int >(instance.m_user_1_status);
    deserializer->ReadRaw< int >(instance.m_user_2_status);
    deserializer->ReadString(instance.m_note);
    deserializer->ReadRaw< int >(instance.m_conversation_id);

    int created_at;
    deserializer->ReadRaw< int >(created_at);
    instance.m_created_at = date::sys_seconds(std::chrono::seconds(created_at));
}