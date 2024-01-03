#include "MessageManager.hpp"
#include "UserManager.hpp"

MessageManager::MessageManager() {
    load_messages();
    // do smth with the next id counter
}

MessageManager::~MessageManager() { save_messages(); }

int MessageManager::add_message(Message message) {
    message.set_msg_id(++m_next_id);
    m_messages.push_back(message);
    return m_next_id;
}

bool MessageManager::delete_message(int message_id) {
    int pos = findById(message_id);
    if (pos != -1) {
        m_messages.erase(m_messages.begin() + pos);
        return true;
    }
    return false;
}

bool MessageManager::update_message(Message message) {
    int pos = findById(message.get_msg_id());
    if (pos != -1) {
        m_messages[pos] = message;
        return true;
    }
    return false;
}

Message MessageManager::get_message(int message_id) {
    int pos = findById(message_id);
    if (pos != -1) {
        return m_messages[pos];
    }
    return Message();
}

std::vector< Message > MessageManager::get_messages() { return m_messages; }

std::optional<Message> MessageManager::send_message(int sender_id, int receiver_id,
                                     std::string content) {
    if(UserManager::getInstance().exists(sender_id) == false) {
        return std::nullopt;
    }

    Message message{++m_next_id, sender_id, receiver_id, content, MessageType::Text};
    m_messages.push_back(message);
    return message;
}

void MessageManager::save_messages() {
    // ...
}

void MessageManager::load_messages() {
    // ...
}

int MessageManager::findById(int id) {
    for (int i = 0; i < m_messages.size(); i++) {
        if (m_messages[i].get_msg_id() == id) {
            return i;
        }
    }
    return -1;
}