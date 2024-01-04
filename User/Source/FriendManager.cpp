#include "FriendManager.hpp"

#include "UserManager.hpp"

FriendManager& FriendManager::getInstance() {
    static FriendManager instance;  // Guaranteed to be destroyed.
                                    // Instantiated on first use.
    return instance;
}

FriendManager::FriendManager() {
    load_friends();
}

FriendManager::~FriendManager() { save_friends(); }

std::optional< Friend > FriendManager::add_friend(int user_1_id, int user_2_id,
                                                  std::string note) {
    if (is_friend(user_1_id, user_2_id)) return std::nullopt;
    if (is_blocked(user_1_id, user_2_id)) return std::nullopt;

    Friend friend_;
    friend_.set_friend_id(++m_next_id)
        .set_user_1_id(user_1_id)
        .set_user_2_id(user_2_id)
        .set_user_1_status(1)
        .set_user_2_status(0)
        .set_note(note)
        .set_conversation_id(-1);

    m_friends.push_back(friend_);
    return friend_;
}

std::optional< Friend > FriendManager::accept_friend(int user_1_id,
                                                     int user_2_id) {
    int pos = findById(user_1_id, user_2_id);
    if (pos == -1) return std::nullopt;

    m_friends[pos].set_user_1_status(1).set_user_2_status(1).set_note();

    

    return m_friends[pos];
}

bool FriendManager::decline_friend(int user_1_id, int user_2_id) {
    int pos = findById(user_1_id, user_2_id);
	if (pos == -1) return false;

	m_friends.erase(m_friends.begin() + pos);
	return true;
}

bool FriendManager::unfriend(int user_1_id, int user_2_id) {
    int pos = findById(user_1_id, user_2_id);
    if (pos == -1) return false;

    m_friends.erase(m_friends.begin() + pos);
    return true;
}

bool FriendManager::block(int user_1_id, int user_2_id) {
    int pos = findById(user_1_id, user_2_id);
    if (pos != -1) {
        m_friends.erase(m_friends.begin() + pos);
    }

    Friend friend_;
    friend_.set_friend_id(++m_next_blacklist_id)
        .set_user_1_id(user_1_id)
        .set_user_2_id(user_2_id)
        .set_user_1_status(1)
        .set_user_2_status(0)
        .set_note()
        .set_conversation_id(-1);

    m_blacklist.push_back(friend_);

    return true;
}

std::optional< Friend > FriendManager::set_direct_message_id(
    int user_1_id, int user_2_id,
                                          int conversation_id) {
	int pos = findById(user_1_id, user_2_id);
    if (pos == -1) return std::nullopt;

    m_friends[pos].set_conversation_id(conversation_id);

    return m_friends[pos];
}

std::optional< Friend > FriendManager::get_friend(int friend_id) {
    int pos = findById(friend_id);
    if (pos == -1) return std::nullopt;

    return m_friends[pos];
}

std::optional< Friend > FriendManager::get_friend(int user_1_id,
                                                  int user_2_id) {
    int pos = findById(user_1_id, user_2_id);
    if (pos == -1) return std::nullopt;

    return m_friends[pos];
}

std::vector< User > FriendManager::get_friends(int user_id) {
    std::vector< int > friends_id = get_friends_id(user_id);
    std::vector< User > friends;
    for (auto& friend_id : friends_id) {
        friends.push_back(
            UserManager::getInstance().get_user(friend_id).value());
    }

    return friends;
}

std::vector< int > FriendManager::get_friends_id(int user_id) {
    std::vector< int > friends;
    for (auto& friend_ : m_friends) {
        if (friend_.get_user_1_id() != user_id &&
            friend_.get_user_2_id() != user_id)
            continue;

        int user_id_ = (friend_.get_user_1_id() == user_id)
                          ? friend_.get_user_2_id()
                          : friend_.get_user_1_id();

        friends.push_back(user_id_);
    }

    return friends;
}

std::vector< Friend > FriendManager::get_friends() { return m_friends; }

std::vector< Friend > FriendManager::get_friend_requests(int user_id) {
    std::vector< Friend > friend_requests;
    for (auto& friend_ : m_friends) {
        if (friend_.get_user_2_id() == user_id &&
            friend_.get_user_2_status() == 0) {
            friend_requests.push_back(friend_);
        }
    }

    return friend_requests;
}

std::vector< Friend > FriendManager::get_blacklist(int user_id) {
    std::vector< Friend > blacklist;
    for (auto& friend_ : m_blacklist) {
        if (friend_.get_user_1_id() == user_id) {
            blacklist.push_back(friend_);
        }
    }

    return blacklist;
}

std::vector< User > FriendManager::get_mutual_friends(int user_1_id,
                                                      int user_2_id) {
    std::vector< User > mutual_friends;
    for (auto& friend_ : m_friends) {
        int cnt = 0;
        if (friend_.get_user_1_id() == user_1_id ||
            friend_.get_user_2_id() == user_1_id) {
            cnt++;
        }

        if (friend_.get_user_1_id() == user_2_id ||
            friend_.get_user_2_id() == user_2_id) {
            cnt++;
        }

        if (cnt != 1) continue;

        if (friend_.get_user_1_id() == user_1_id ||
            friend_.get_user_2_id() == user_1_id) {
            mutual_friends.push_back(UserManager::getInstance()
                                         .get_user(friend_.get_user_2_id())
                                         .value());
        } else {
            mutual_friends.push_back(UserManager::getInstance()
                                         .get_user(friend_.get_user_1_id())
                                         .value());
        }
    }

    return mutual_friends;
}

std::vector< User > FriendManager::get_friends_of_friends(int user_id) {
    std::vector< User > friends = get_friends(user_id);

    std::set< int > friend_ids;

    for (auto& user : friends) {
        friend_ids.insert(user.get_user_id());
    }

    std::vector< User > friends_of_friends;

    for (auto& user : friends) {
        std::vector< User > friends_ = get_friends(user.get_user_id());
        for (auto& friend_ : friends_) {
            if (friend_.get_user_id() == user_id) continue;
            if (friend_ids.find(friend_.get_user_id()) == friend_ids.end()) {
                friends_of_friends.push_back(friend_);
            }
        }
    }

    return friends_of_friends;
}

bool FriendManager::is_friend(int user_1_id, int user_2_id) {
    int pos = findById(user_1_id, user_2_id);
    if (pos != -1) return true;
    return false;
}

bool FriendManager::is_blocked(int user_1_id, int user_2_id) {
    for (auto& friend_ : m_blacklist) {
        if (friend_.get_user_1_id() == user_1_id &&
            friend_.get_user_2_id() == user_2_id) {
            return true;
        }
    }

    return false;
}

bool FriendManager::is_friend(int friend_id) {
    int pos = findById(friend_id);
    if (pos != -1) return true;
    return false;
}

void FriendManager::save_friends() {
    YAML::Emitter out;
    out << YAML::BeginSeq;

    out << m_friends.size();

    for (auto& friend_ : m_friends) {
        out << friend_;
    }

    out << m_next_id;

    out << m_blacklist.size();

    for (auto& friend_ : m_blacklist) {
        out << friend_;
    }

    out << m_next_blacklist_id;

    out << YAML::EndSeq;

    std::ofstream fout(m_friends_file);
    fout << out.c_str();

    fout.close();
}

void FriendManager::load_friends() {
    std::ifstream fin(m_friends_file);

    if (!fin.is_open()) {
        std::cout << "File not found!\n";
        return;
    }

    YAML::Node doc = YAML::Load(fin);

    int size = doc[0].as< int >();

    for (int i = 0; i < size; i++) {
        Friend friend_;
        doc[i + 1] >> friend_;
        m_friends.push_back(friend_);
    }

    m_next_id = doc[size + 1].as< int >();

    size = doc[size + 2].as< int >();

    for (int i = 0; i < size; i++) {
        Friend friend_;
        doc[i + size + 3] >> friend_;
        m_blacklist.push_back(friend_);
    }

    m_next_blacklist_id = doc[size + size + 3].as< int >();
}

int FriendManager::findById(int friend_id) {
    for (int i = 0; i < m_friends.size(); i++) {
        if (m_friends[i].get_friend_id() == friend_id) {
            return i;
        }
    }
    return -1;
}

int FriendManager::findById(int user_1_id, int user_2_id) {
    for (int i = 0; i < m_friends.size(); i++) {
        if ((m_friends[i].get_user_1_id() == user_1_id &&
             m_friends[i].get_user_2_id() == user_2_id) ||
            (m_friends[i].get_user_1_id() == user_2_id &&
             m_friends[i].get_user_2_id() == user_1_id)) {
            return i;
        }
    }
    return -1;
}