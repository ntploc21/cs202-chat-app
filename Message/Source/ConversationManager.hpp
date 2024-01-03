#pragma once

#include <vector>
#include "Conversation.hpp"

class ConversationManager {
public:
    ConversationManager(ConversationManager const&) = delete;
    void operator=(ConversationManager const&) = delete;
    static ConversationManager& getInstance() {
        static ConversationManager instance;  // Guaranteed to be destroyed.
                                      // Instantiated on first use.
        return instance;
    }

private:
    ConversationManager();
    ~ConversationManager();

public:

    int add_conversation(Conversation conversation);
	bool delete_conversation(int conversation_id);
	bool update_conversation(Conversation conversation);
	std::optional< Conversation > get_conversation(int conversation_id);
	std::vector< Conversation > get_conversations();
    
    std::optional< Message > send_message(int sender_id, int conversation_id, Message message);
    std::optional< Message > send_message(int sender_id, int conversation_id, std::string message);

private:
    void save_conversations();
	void load_conversations();

	int findById(int id);

private:
	std::vector< Conversation > m_conversations{};
    int m_next_id{};
};