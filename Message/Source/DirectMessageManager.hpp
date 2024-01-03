#pragma once

#include "DirectMessage.hpp"

class DirectMessageManager {
public:
    DirectMessageManager(DirectMessageManager const&) = delete;
    void operator=(DirectMessageManager const&) = delete;
    static DirectMessageManager& getInstance() {
        static DirectMessageManager instance;  // Guaranteed to be destroyed.
                                               // Instantiated on first use.
        return instance;
    }

private:
    DirectMessageManager();
    ~DirectMessageManager();

public:
    int add_direct_message(DirectMessage direct_message);
    bool delete_direct_message(int direct_message_id);
    bool update_direct_message(DirectMessage direct_message);
    DirectMessage get_direct_message(int direct_message_id);
    std::vector< DirectMessage > get_direct_messages();

    std::optional< Message > send_message(int sender_id, int receiver_id,
                                          std::string content);

private:
    void save_direct_messages();
    void load_direct_messages();

    int findById(int id);

private:
    std::vector< DirectMessage > m_direct_messages{};
    int m_next_id{0};
};