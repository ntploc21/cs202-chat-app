#include "GroupMessageManager.hpp"

GroupMessageManager::GroupMessageManager() {
	load_group_messages();
}

GroupMessageManager::~GroupMessageManager() {
	save_group_messages();
}

int GroupMessageManager::add_group_message(GroupMessage group_message) {
    group_message.set_group_id(++m_next_id);
	m_group_messages.push_back(group_message);
    return m_next_id;
}

bool GroupMessageManager::delete_group_message(int group_message_id) {
	int pos = findById(group_message_id);
	if (pos == -1) return false;
	m_group_messages.erase(m_group_messages.begin() + pos);
	return true;
}

bool GroupMessageManager::update_group_message(GroupMessage group_message) {
	int pos = findById(group_message.get_group_id());
	if (pos == -1) return false;
	m_group_messages[pos] = group_message;
	return true;
}

GroupMessage GroupMessageManager::get_group_message(int group_message_id) {
	int pos = findById(group_message_id);
	if (pos == -1) return GroupMessage();
	return m_group_messages[pos];
}

std::vector< GroupMessage > GroupMessageManager::get_group_messages() {
	return m_group_messages;
}

std::optional< Message > GroupMessageManager::send_message(int sender_id, int group_id, std::string content) {
	int pos = findById(group_id);
	if (pos == -1) return std::nullopt;
	return m_group_messages[pos].send_message(sender_id, content);
}

void GroupMessageManager::save_group_messages() {
	// ...
}

void GroupMessageManager::load_group_messages() {
	// ...
}

int GroupMessageManager::findById(int id) {
	for (int i = 0; i < m_group_messages.size(); i++) {
			if (m_group_messages[i].get_group_id() == id) {
						return i;
					}
		}
	return -1;
}