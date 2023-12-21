#include "User.hpp"

#include "User-Utils.hpp"

User::User(int user_id, std::string username, std::string password,
           std::string fullname, bool active, bool online) {
    if (!validate_user_password(password)) {
        throw std::invalid_argument("Invalid password");
    }

    this->user_id = user_id;
    this->username = username;
    this->password = password;
    this->fullname = fullname;
    this->active = active;
    this->online = online;
}

int User::get_user_id() const { return this->user_id; }

std::string User::get_username() const { return this->username; }

std::string User::get_password() const { return this->password; }

std::string User::get_fullname() const { return this->fullname; }

bool User::is_active() const { return this->active; }

bool User::is_online() const { return this->online; }

std::vector< int > User::get_group_list() const { return this->group_list; }

std::vector< int > User::get_friend_list() const { return this->friend_list; }

User& User::set_user_id(int user_id) {
    this->user_id = user_id;
    return *this;
}

User& User::set_username(std::string username) {
    this->username = username;
    return *this;
}

User& User::set_password(std::string password) {
    if (!validate_user_password(password)) {
        throw std::invalid_argument("Invalid password");
    }
    this->password = password;
    return *this;
}

User& User::set_fullname(std::string fullname) {
    this->fullname = fullname;
    return *this;
}

User& User::set_active(bool active) {
    this->active = active;
    return *this;
}

User& User::set_online(bool online) {
    this->online = online;
    return *this;
}

User& User::set_group_list(std::vector< int > group_list) {
    this->group_list = group_list;
    return *this;
}

User& User::set_friend_list(std::vector< int > friend_list) {
    this->friend_list = friend_list;
    return *this;
}

User& User::add_group(int group_id) {
    this->group_list.push_back(group_id);
    return *this;
}

User& User::add_friend(int friend_id) {
    this->friend_list.push_back(friend_id);
    return *this;
}

User& User::remove_group(int group_id) {
    this->group_list.erase(
        std::remove(this->group_list.begin(), this->group_list.end(), group_id),
        this->group_list.end());
    return *this;
}

User& User::remove_friend(int friend_id) {
    this->friend_list.erase(std::remove(this->friend_list.begin(),
                                        this->friend_list.end(), friend_id),
                            this->friend_list.end());
    return *this;
}

std::ostream& operator<<(std::ostream& out, const User& user) {
    out << "User ID: " << user.user_id << "\n";
    out << "Username: " << user.username << "\n";
    out << "Password: " << user.password << "\n";
    out << "Fullname: " << user.fullname << "\n";
    out << "Is active: " << user.active << "\n";
    out << "Is online: " << user.online << "\n";

    out << "Groups: \n";
    for (int group : user.group_list) {
        out << "- " << group << "\n";
    }

    out << "Friends: \n";
    for (int friend_id : user.friend_list) {
        out << "- " << friend_id << "\n";
    }

    return out;
}