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
    int new_direct_message(int user_1_id, int user_2_id);

    bool add_message(int direct_message_id, Message message);

    bool delete_direct_message(int direct_message_id);
    bool update_direct_message(DirectMessage direct_message);
    std::optional< DirectMessage > get_direct_message(int direct_message_id);
    std::vector< DirectMessage > get_direct_messages();
    std::vector< DirectMessage > get_direct_messages(int user_id);

    std::optional< Message > send_message(int sender_id, int receiver_id,
                                          std::string content);

    void set_used_by_client();

    void load_direct_messages(std::vector< DirectMessage > direct_messages);

    void clear_data();
private:
    void save_direct_messages();
    void load_direct_messages();

    int findById(int id);

private:
    std::vector< DirectMessage > m_direct_messages{};
    int m_next_id{0};

    std::string_view m_direct_messages_file{"direct_messages.yaml"};

    bool m_used_by_client = false;
};