#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Walnut/Serialization/StreamReader.h"
#include "Walnut/Serialization/StreamWriter.h"


#include "yaml-cpp/yaml.h"

class User {
public:
    // make it as a builder using builder design pattern
    User();
    User(std::string username, std::string password, std::string fullname,
         int user_id = 0, bool active = 1, bool online = 1);
    virtual ~User() = default;
    int get_user_id() const;
    std::string get_username() const;
    std::string get_password() const;
    std::string get_fullname() const;
    bool is_active() const;
    bool is_online() const;
    std::vector< int > get_group_list() const;
    User& set_user_id(int user_id);
    User& set_username(std::string username);
    User& set_password(std::string password);
    User& set_fullname(std::string fullname);
    User& set_active(bool active);
    User& set_online(bool online);
    User& set_group_list(std::vector< int > group_list);
    User& add_group(int group_id);
    //User& add_friend(int friend_id);
    User& remove_group(int group_id);

    friend std::ostream& operator<<(std::ostream& out, const User& user);

    static void Serialize(Walnut::StreamWriter* serializer,
                          const User& instance);

    static void Deserialize(Walnut::StreamReader* deserializer,
                            User& instance);

    friend YAML::Emitter& operator<<(YAML::Emitter& out, const User& user);

    friend void operator>>(const YAML::Node& in, User& user);

    bool operator==(const User& other) const;


public:

    int m_user_id{};
    std::string m_username{};
    std::string m_password{};
    std::string m_fullname{};
    bool m_active{};
    bool m_online{};
    // date_of_birth
    // profile_photo
    std::vector< int > m_friend_list{};
    std::vector< int > m_group_list{};
};

bool validate_user_password(std::string_view password);