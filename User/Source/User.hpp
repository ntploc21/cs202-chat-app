#pragma once
#include <iostream>
#include <string>
#include <vector>

class User {
public:
    // make it as a builder using builder design pattern
    User(int user_id, std::string username, std::string password,
         std::string fullname, bool active, bool online);
    virtual ~User() = default;
    int get_user_id() const;
    std::string get_username() const;
    std::string get_password() const;
    std::string get_fullname() const;
    bool is_active() const;
    bool is_online() const;
    std::vector< int > get_group_list() const;
    std::vector< int > get_friend_list() const;
    User& set_user_id(int user_id);
    User& set_username(std::string username);
    User& set_password(std::string password);
    User& set_fullname(std::string fullname);
    User& set_active(bool active);
    User& set_online(bool online);
    User& set_group_list(std::vector< int > group_list);
    User& set_friend_list(std::vector< int > friend_list);
    User& add_group(int group_id);
    User& add_friend(int friend_id);
    User& remove_group(int group_id);
    User& remove_friend(int friend_id);

    friend std::ostream& operator<<(std::ostream& out, const User& user);

private:
    int m_user_id{};
    std::string m_username{};
    std::string m_password{};
    std::string m_fullname{};
    bool m_active{};
    bool m_online{};
    // date_of_birth
    // profile_photo
    std::vector< int > m_group_list{};
    std::vector< int > m_friend_list{};
};