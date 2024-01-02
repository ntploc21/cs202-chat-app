#include "User.hpp"

#include "User-Utils.hpp"

User::User() {}

User::User(std::string username, std::string password, std::string fullname,
           int user_id, bool active, bool online) {
    if (!validate_user_password(password)) {
        throw std::invalid_argument("Invalid password");
    }

    this->m_user_id = user_id;
    this->m_username = username;
    this->m_password = password;
    this->m_fullname = fullname;
    this->m_active = active;
    this->m_online = online;
}

int User::get_user_id() const { return this->m_user_id; }

std::string User::get_username() const { return this->m_username; }

std::string User::get_password() const { return this->m_password; }

std::string User::get_fullname() const { return this->m_fullname; }

bool User::is_active() const { return this->m_active; }

bool User::is_online() const { return this->m_online; }

std::vector< int > User::get_group_list() const { return this->m_group_list; }

User& User::set_user_id(int user_id) {
    this->m_user_id = user_id;
    return *this;
}

User& User::set_username(std::string username) {
    this->m_username = username;
    return *this;
}

User& User::set_password(std::string password) {
    if (!validate_user_password(password)) {
        throw std::invalid_argument("Invalid password");
    }
    this->m_password = password;
    return *this;
}

User& User::set_fullname(std::string fullname) {
    this->m_fullname = fullname;
    return *this;
}

User& User::set_active(bool active) {
    this->m_active = active;
    return *this;
}

User& User::set_online(bool online) {
    this->m_online = online;
    return *this;
}

User& User::set_group_list(std::vector< int > group_list) {
    this->m_group_list = group_list;
    return *this;
}

User& User::add_group(int group_id) {
    this->m_group_list.push_back(group_id);
    return *this;
}

User& User::remove_group(int group_id) {
    this->m_group_list.erase(std::remove(this->m_group_list.begin(),
                                         this->m_group_list.end(), group_id),
                             this->m_group_list.end());
    return *this;
}

std::ostream& operator<<(std::ostream& out, const User& user) {
    out << "User ID: " << user.m_user_id << "\n";
    out << "Username: " << user.m_username << "\n";
    out << "Password: " << user.m_password << "\n";
    out << "Fullname: " << user.m_fullname << "\n";
    out << "Is active: " << user.m_active << "\n";
    out << "Is online: " << user.m_online << "\n";

    out << "Groups: \n";
    for (int group : user.m_group_list) {
        out << "- " << group << "\n";
    }

    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const User& user) {
    out << YAML::BeginMap;

    out << YAML::Key << "user_id" << YAML::Value << user.m_user_id;

    out << YAML::Key << "username" << YAML::Value << user.m_username;

    out << YAML::Key << "password" << YAML::Value << user.m_password;

    out << YAML::Key << "fullname" << YAML::Value << user.m_fullname;

    out << YAML::Key << "active" << YAML::Value << user.m_active;

    out << YAML::Key << "online" << YAML::Value << user.m_online;

    out << YAML::Key << "group_list" << YAML::Value << user.m_group_list;

    out << YAML::EndMap;

    return out;
}

void operator>>(const YAML::Node& in, User& user) {
    user.m_user_id = in["user_id"].as< int >();
    user.m_username = in["username"].as< std::string >();
    user.m_password = in["password"].as< std::string >();
    user.m_fullname = in["fullname"].as< std::string >();
    user.m_active = in["active"].as< bool >();
    user.m_online = in["online"].as< bool >();
    user.m_group_list = in["group_list"].as< std::vector< int > >();
}

void User::Serialize(Walnut::StreamWriter* serializer, const User& instance) {
    serializer->WriteRaw< int >(instance.m_user_id);
    serializer->WriteString(instance.m_username);
    serializer->WriteString(instance.m_password);
    serializer->WriteString(instance.m_fullname);
    serializer->WriteRaw< bool >(instance.m_active);
    serializer->WriteRaw< bool >(instance.m_online);
    serializer->WriteArray(instance.m_friend_list);
    serializer->WriteArray(instance.m_group_list);
}

void User::Deserialize(Walnut::StreamReader* deserializer, User& instance) {
    deserializer->ReadRaw< int >(instance.m_user_id);
    deserializer->ReadString(instance.m_username);
    deserializer->ReadString(instance.m_password);
    deserializer->ReadString(instance.m_fullname);
    deserializer->ReadRaw< bool >(instance.m_active);
    deserializer->ReadRaw< bool >(instance.m_online);
    deserializer->ReadArray(instance.m_friend_list);
    deserializer->ReadArray(instance.m_group_list);
}