#pragma once

#include <vector>

#include "Message.hpp"

class MessageManager {
public:
    MessageManager(MessageManager const&) = delete;
    void operator=(MessageManager const&) = delete;
    static MessageManager& getInstance() {
        static MessageManager instance;  // Guaranteed to be destroyed.
                                         // Instantiated on first use.
        return instance;
    }

private:
    MessageManager();
    ~MessageManager();

public:
    int add_message(Message message);
    bool delete_message(int message_id);
    bool update_message(Message message);
    std::optional< Message > get_message(int message_id);
    std::vector< Message > get_messages();

    std::optional< Message > send_message(int sender_id, int receiver_id,
                                          std::string content);

    std::optional< Message > send_announcement(int sender_id, int receiver_id,
        											   std::string content);

    void set_used_by_client();

    void insert_messages(std::vector< Message > messages);

    void clear_data();
private:
    void save_messages();
    void load_messages();

    int findById(int id);

private:
    std::vector< Message > m_messages{};
    int m_next_id{0};

    std::string_view m_messages_file{"messages.yaml"};

    bool m_used_by_client = false;
};