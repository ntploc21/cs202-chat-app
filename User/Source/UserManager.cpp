#include "UserManager.hpp"

#include "FriendManager.hpp"

UserManager::UserManager() { load_users(); }

UserManager::~UserManager() { save_users(); }

int UserManager::add_user(User user) {
    int user_pos = findByUsername(user.get_username());
    if (user_pos != -1) return m_users[user_pos].get_user_id();

    user.set_user_id(++m_next_id);
    m_users.push_back(user);
    return m_next_id;
}

bool UserManager::delete_user(int user_id) {
    int pos = findById(user_id);
    if (pos != -1) {
        m_users.erase(m_users.begin() + pos);
        return true;
    }
    return false;
}

bool UserManager::update_user(User user) {
    int pos = findById(user.get_user_id());
    if (pos != -1) {
        m_users[pos] = user;
        return true;
    }
    return false;
}

std::optional< User > UserManager::get_user(int user_id) {
    int pos = findById(user_id);
    if (pos == -1) return std::nullopt;
    User user = m_users[pos];
    user.m_friend_list = FriendManager::getInstance().get_friends_id(user_id);
}

std::vector< User > UserManager::filter(
    std::unique_ptr< ISpecification< User > >& spec) {
    UserFilter user_filter;

    return user_filter.filter(m_users, spec);
}

std::optional< User > UserManager::findByUsernameAndPassword(
    std::string_view username, std::string_view password) {
    int pos = findByUsername(username);
    if (pos != -1) {
        if (m_users[pos].get_password() == password) {
            return m_users[pos];
        }
    }

    return std::nullopt;
}

bool UserManager::exists(std::string_view username) {
    int pos = findByUsername(username);
    if (pos != -1) return true;
    return false;
}

bool UserManager::exists(int user_id) {
    int pos = findById(user_id);
    if (pos != -1) return true;
    return false;
}

std::vector< User > UserManager::get_users() { return m_users; }

void UserManager::save_users() {
    // output to m_users_file

    YAML::Emitter out;

    out << YAML::BeginSeq;

    for (int i = 0; i < m_users.size(); i++) {
        out << m_users[i];
    }

    out << m_next_id;

    out << YAML::EndSeq;

    std::ofstream fout(m_users_file);

    fout << out.c_str();

    fout.close();
}

void UserManager::load_users() {
    // input from m_users_file

    std::ifstream fin(m_users_file);

    if (!fin.is_open()) {
        std::cout << "File not found!\n";
        return;
    }

    YAML::Node doc = YAML::Load(fin);

    for (int i = 0; i < doc.size() - 1; i++) {
        User user;
        doc[i] >> user;
        m_users.push_back(user);
    }

    m_next_id = doc[doc.size() - 1].as< int >();
}

int UserManager::findById(int user_id) {
    for (int i = 0; i < m_users.size(); i++) {
        if (m_users[i].get_user_id() == user_id) {
            return i;
        }
    }
    return -1;
}

int UserManager::findByUsername(std::string_view username) {
    for (int i = 0; i < m_users.size(); i++) {
        if (m_users[i].get_username() == username) {
            return i;
        }
    }
    return -1;
}