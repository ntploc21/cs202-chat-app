#include "ConversationManager.hpp"

#include "MessageManager.hpp"

ConversationManager::ConversationManager() {
    load_conversations();
    // do smth with the next id counter
}

ConversationManager::~ConversationManager() { save_conversations(); }

int ConversationManager::add_conversation(Conversation conversation) {
    conversation.set_conversation_id(++m_next_id);
    m_conversations.push_back(conversation);
    return m_next_id;
}

bool ConversationManager::delete_conversation(int conversation_id) {
    int pos = findById(conversation_id);
    if (pos != -1) {
        m_conversations.erase(m_conversations.begin() + pos);
        return true;
    }
    return false;
}

bool ConversationManager::update_conversation(Conversation conversation) {
    int pos = findById(conversation.get_conversation_id());
    if (pos != -1) {
        m_conversations[pos] = conversation;
        return true;
    }
    return false;
}

std::optional< Conversation > ConversationManager::get_conversation(
    int conversation_id) {
    int pos = findById(conversation_id);
    if (pos != -1) {
        return m_conversations[pos];
    }
    return std::nullopt;
}

std::vector< Conversation > ConversationManager::get_conversations() {
    return m_conversations;
}

std::optional< Message > ConversationManager::send_message(int sender_id,
                                                           int conversation_id,
                                                           Message message) {
    int pos = findById(conversation_id);
    if(pos == -1) return std::nullopt;

    auto msg = m_conversations[pos].send_message(sender_id, message.get_content());
    if(!msg.has_value()) return std::nullopt;

    return msg;
}

std::optional< Message > ConversationManager::send_message(
    int sender_id, int conversation_id, std::string message) {
    int pos = findById(conversation_id);
    if(pos == -1) return std::nullopt;

    auto msg = m_conversations[pos].send_message(sender_id, message);
    if(!msg.has_value()) return std::nullopt;

    return msg;
}

void ConversationManager::save_conversations() {
    // ...
}

void ConversationManager::load_conversations() {
    // ...
}

int ConversationManager::findById(int id) {
    for (int i = 0; i < m_conversations.size(); i++) {
        if (m_conversations[i].get_conversation_id() == id) {
            return i;
        }
    }
    return -1;
}
