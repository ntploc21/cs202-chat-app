#include "Conversation.hpp"

#include "MessageManager.hpp"

Conversation::Conversation() {}

Conversation& Conversation::set_conversation_id(int conversation_id) {
    m_conversation_id = conversation_id;
    return *this;
}

int Conversation::get_conversation_id() const {
	return m_conversation_id;
}

std::vector< int > Conversation::get_messages_id() const {
	return m_messages;
}

std::vector< Message > Conversation::get_messages() const {
	std::vector< Message > messages{};
	for (int i = 0; i < m_messages.size(); i++) {
			messages.push_back(MessageManager::getInstance().get_message(m_messages[i]));
		}
	return messages;
}

Conversation& Conversation::add_message(int message_id) {
	m_messages.push_back(message_id);
	return *this;
}

Conversation& Conversation::add_message(Message message) {
	m_messages.push_back(message.get_msg_id());
	return *this;
}

std::optional< Message > Conversation::send_message(int sender_id,
                                                    std::string content) {
	auto msg = MessageManager::getInstance().send_message(
			sender_id, m_conversation_id, content);

	if (!msg.has_value()) return std::nullopt;

	m_messages.push_back(msg.value().get_msg_id());
	return msg;
}

Conversation& Conversation::remove_message(int message_id) {
	for (int i = 0; i < m_messages.size(); i++) {
			if (m_messages[i] == message_id) {
						m_messages.erase(m_messages.begin() + i);
						return *this;
					}
		}
	return *this;
}

Conversation& Conversation::remove_message(Message message) {
	for (int i = 0; i < m_messages.size(); i++) {
				if (m_messages[i] == message.get_msg_id()) {
										m_messages.erase(m_messages.begin() + i);
										return *this;
									}
			}
	return *this;
}

Conversation& Conversation::clear_messages() {
	m_messages.clear();
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Conversation& conversation) {
	out << "Conversation: " << conversation.m_conversation_id << std::endl;
	out << "Messages: ";
	for (int i = 0; i < conversation.m_messages.size(); i++) {
				out << conversation.m_messages[i] << " ";
			}
	out << std::endl;
	return out;
}

void Conversation::Serialize(Walnut::StreamWriter* serializer,
		const Conversation& instance) {
	serializer->WriteRaw< int >(instance.m_conversation_id);
	serializer->WriteArray(instance.m_messages);
}

void Conversation::Deserialize(Walnut::StreamReader* deserializer,
			Conversation& instance) {
	deserializer->ReadRaw< int >(instance.m_conversation_id);
	deserializer->ReadArray(instance.m_messages);
}