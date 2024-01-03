#pragma once

#include <map>
#include <vector>
#include <string>
#include "User.hpp"
#include "Message.hpp"

class DirectMessage {
public:
	DirectMessage();
	DirectMessage(int dm_id, int conversation_id, int user_id_1, int user_id_2, std::string user_1_nickname = "", std::string user_2_nickname = "");
	virtual ~DirectMessage() = default;

	DirectMessage& set_dm_id(int dm_id);
	int get_dm_id() const;

	DirectMessage& set_conversation_id(int conversation_id);
	int get_conversation_id() const;

	DirectMessage& set_user_id_1(int user_id_1);
	int get_user_id_1() const;

	DirectMessage& set_user_id_2(int user_id_2);
	int get_user_id_2() const;

	DirectMessage& set_user_nickname(int user_id, std::string nickname);

	std::string get_user_nickname(int user_id) const;

	std::vector<int> get_user_list_ids() const;

	DirectMessage& add_pin_message(int message_id);
	DirectMessage& remove_pin_message(int message_id);
	std::vector<int> get_pin_message_list_ids() const;

	bool is_pin_message(int message_id) const;

	friend std::ostream& operator<<(std::ostream& out,
						const DirectMessage& direct_message);

	static void Serialize(Walnut::StreamWriter* serializer,
						const DirectMessage& instance);

	static void Deserialize(Walnut::StreamReader* deserializer,
						DirectMessage& instance);

public:
	std::optional< Message > send_message(int sender_id, Message message) const;
	std::optional< Message> send_message(int sender_id, std::string message) const;

private:
	int m_dm_id{};
	int m_conversation_id{};
	int m_user_id_1{};
	int m_user_id_2{};
	std::string m_user_1_nickname{};
	std::string m_user_2_nickname{};
	std::vector<int> m_pin_message_list{};
	// images
	// attachments
};