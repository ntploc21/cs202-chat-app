#pragma once

#include <optional>

#include "Friend.hpp"
#include "User.hpp"

class FriendManager {
public:
    FriendManager(FriendManager const&) = delete;
    void operator=(FriendManager const&) = delete;
    static FriendManager& getInstance() {
        static FriendManager instance;  // Guaranteed to be destroyed.
                                      // Instantiated on first use.
        return instance;
    }

private:
    FriendManager();
    ~FriendManager();

public:
    std::optional< Friend > add_friend(int user_1_id, int user_2_id, std::string note = "");
    std::optional< Friend > accept_friend(int user_1_id, int user_2_id);
    bool unfriend(int user_1_id, int user_2_id);
    bool block(int user_1_id, int user_2_id);
    
    std::optional< Friend > get_friend(int friend_id);
    std::optional< Friend > get_friend(int user_1_id, int user_2_id);
    std::vector< User > get_friends(int user_id);
    std::vector< int > get_friends_id(int user_id);
    std::vector< Friend > get_friends();
    std::vector< Friend > get_friend_requests(int user_id);
    std::vector< Friend > get_blacklist(int user_id);
    std::vector< User > get_mutual_friends(int user_1_id, int user_2_id);
    std::vector< User > get_friends_of_friends(int user_id);

    bool is_friend(int user_1_id, int user_2_id);
    bool is_blocked(int user_1_id, int user_2_id);
    bool is_friend(int friend_id);

private:
    void save_friends();
	void load_friends();

	int findById(int id);
    int findById(int user_1_id, int user_2_id);

private:
    std::vector< Friend > m_friends{}; // bidirectional
    int m_next_id{0};
    
    std::vector< Friend > m_blacklist{}; // unidirectional
    int m_next_blacklist_id{0};

    const std::string m_friends_file{"friends.yaml"};
};