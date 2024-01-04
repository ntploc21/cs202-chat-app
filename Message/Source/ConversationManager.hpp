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
    int new_conversation();
	bool delete_conversation(int conversation_id);
	bool update_conversation(Conversation conversation);
	std::optional< Conversation > get_conversation(int conversation_id);
	std::vector< Conversation > get_conversations();
    std::vector< Message > get_messages(int conversation_id);
    std::optional< Message > get_last_message(int conversation_id);
    
    std::optional< Message > send_message(int sender_id, int conversation_id, Message message);
    std::optional< Message > send_message(int sender_id, int conversation_id, std::string message);
    
    void set_used_by_client();
    
    void insert_conversations(std::vector< Conversation > conversations);

private:
    void save_conversations();
	void load_conversations();

	int findById(int id);

private:
	std::vector< Conversation > m_conversations{};
    int m_next_id{};

    std::string_view m_conversation_file{"conversations.yaml"};

    bool m_used_by_client = false;
};