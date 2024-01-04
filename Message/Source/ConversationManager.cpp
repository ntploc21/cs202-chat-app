#include "ConversationManager.hpp"

#include "MessageManager.hpp"

#include <iostream>

ConversationManager::ConversationManager() { load_conversations(); }

ConversationManager::~ConversationManager() {
    if (!m_used_by_client) save_conversations();
}

int ConversationManager::add_conversation(Conversation conversation) {
    conversation.set_conversation_id(++m_next_id);
    m_conversations.push_back(conversation);
    return m_next_id;
}

int ConversationManager::new_conversation() {
    Conversation conversation;
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

std::vector< Message > ConversationManager::get_messages(int conversation_id) {
    int pos = findById(conversation_id);
    if (pos != -1) {
        return m_conversations[pos].get_messages();
    }
    return std::vector< Message >{};
}

std::optional< Message > ConversationManager::get_last_message(
    int conversation_id) {
    int pos = findById(conversation_id);
    if(pos == -1) return std::nullopt;
    
    return m_conversations[pos].get_last_message();
}

std::optional< Message > ConversationManager::send_message(int sender_id,
                                                           int conversation_id,
                                                           Message message) {
    int pos = findById(conversation_id);
    if (pos == -1) return std::nullopt;

    auto msg =
        m_conversations[pos].send_message(sender_id, message.get_content());
    if (!msg.has_value()) return std::nullopt;

    return msg;
}

std::optional< Message > ConversationManager::send_message(
    int sender_id, int conversation_id, std::string message) {
    if(m_used_by_client) return std::nullopt;

    int pos = findById(conversation_id);
    if (pos == -1) return std::nullopt;

    auto msg = m_conversations[pos].send_message(sender_id, message);
    if (!msg.has_value()) return std::nullopt;

    return msg;
}

void ConversationManager::set_used_by_client() {
    m_conversations.clear();
    m_used_by_client = true;
    m_next_id = 0;
}

void ConversationManager::insert_conversations(
    std::vector< Conversation > conversations) {
    if (!m_used_by_client) return;
    m_conversations.insert(m_conversations.end(), conversations.begin(),
						   conversations.end());
    m_next_id = 0;
}

void ConversationManager::save_conversations() {
    // output to m_conversations_file

    YAML::Emitter out;

    out << YAML::BeginSeq;

    for (auto conversation : m_conversations) {
        out << conversation;
    }

    out << m_next_id;

    out << YAML::EndSeq;

    std::ofstream fout(m_conversation_file.data());

    fout << out.c_str();

    fout.close();
}

void ConversationManager::load_conversations() {
    // input from m_conversations_file

    std::ifstream fin(m_conversation_file.data());

    if (!fin.is_open()) {
        std::cout << "File not found!\n";
        return;
    }

    YAML::Node doc = YAML::Load(fin);

    for (std::size_t i = 0; i < doc.size() - 1; i++) {
        Conversation conversation;
        doc[i] >> conversation;
        m_conversations.push_back(conversation);
    }

    m_next_id = doc[doc.size() - 1].as< int >();
}

int ConversationManager::findById(int id) {
    for (int i = 0; i < m_conversations.size(); i++) {
        if (m_conversations[i].get_conversation_id() == id) {
            return i;
        }
    }
    return -1;
}
