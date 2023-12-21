#include "User.hpp"

#include "User-Utils.hpp"

User::User() {}

User::User(int user_id, std::string username, std::string password,
           std::string fullname, bool active, bool online) {
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

std::vector< int > User::get_friend_list() const { return this->m_friend_list; }

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

User& User::set_friend_list(std::vector< int > friend_list) {
    this->m_friend_list = friend_list;
    return *this;
}

User& User::add_group(int group_id) {
    this->m_group_list.push_back(group_id);
    return *this;
}

User& User::add_friend(int friend_id) {
    this->m_friend_list.push_back(friend_id);
    return *this;
}

User& User::remove_group(int group_id) {
    this->m_group_list.erase(
        std::remove(this->m_group_list.begin(), this->m_group_list.end(), group_id),
        this->m_group_list.end());
    return *this;
}

User& User::remove_friend(int friend_id) {
    this->m_friend_list.erase(std::remove(this->m_friend_list.begin(),
                                        this->m_friend_list.end(), friend_id),
                            this->m_friend_list.end());
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

    out << "Friends: \n";
    for (int friend_id : user.m_friend_list) {
        out << "- " << friend_id << "\n";
    }

    return out;
}