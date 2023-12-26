#include "DirectMessageManager.hpp"

DirectMessageManager::DirectMessageManager() { load_direct_messages(); }

DirectMessageManager::~DirectMessageManager() { save_direct_messages(); }

int DirectMessageManager::add_direct_message(DirectMessage direct_message) {
    direct_message.set_dm_id(++m_next_id);
    m_direct_messages.push_back(direct_message);
    return m_next_id;
}

bool DirectMessageManager::delete_direct_message(int direct_message_id) {
    int pos = findById(direct_message_id);
    if (pos == -1) return false;
    m_direct_messages.erase(m_direct_messages.begin() + pos);
    return true;
}

bool DirectMessageManager::update_direct_message(DirectMessage direct_message) {
    int pos = findById(direct_message.get_dm_id());
    if (pos == -1) return false;
    m_direct_messages[pos] = direct_message;
    return true;
}

DirectMessage DirectMessageManager::get_direct_message(int direct_message_id) {
    int pos = findById(direct_message_id);
    if (pos == -1) return DirectMessage();
    return m_direct_messages[pos];
}

std::vector< DirectMessage > DirectMessageManager::get_direct_messages() {
    return m_direct_messages;
}

std::optional< Message > DirectMessageManager::send_message(
    int sender_id, int receiver_id, std::string content) {
    int pos = findById(receiver_id);
    if (pos == -1) return std::nullopt;
    return m_direct_messages[pos].send_message(sender_id, content);
}

void DirectMessageManager::save_direct_messages() {
    // ...
}

void DirectMessageManager::load_direct_messages() {
    // ...
}

int DirectMessageManager::findById(int id) {
    for (int i = 0; i < m_direct_messages.size(); i++) {
        if (m_direct_messages[i].get_dm_id() == id) {
            return i;
        }
    }
    return -1;
}
