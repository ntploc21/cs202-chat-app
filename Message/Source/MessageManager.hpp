#pragma once

#include "Message.hpp"
#include <vector>

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
	Message get_message(int message_id);
	std::vector< Message > get_messages();

    std::optional<Message> send_message(int sender_id, int receiver_id, std::string content);

private:
    void save_messages();
	void load_messages();

	int findById(int id);

private:
    std::vector< Message > m_messages{};
	int m_next_id{ 0 };
};