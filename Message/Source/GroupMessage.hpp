#pragma once

#include <map>
#include <vector>
#include <string>
#include "User.hpp"
#include "Message.hpp"

class GroupMessage {
public:
	GroupMessage();
	virtual ~GroupMessage() = default;

	GroupMessage& set_group_id(int group_id);
	int get_group_id() const;

	GroupMessage& set_conversation_id(int conversation_id);
	int get_conversation_id() const;

	GroupMessage& set_group_name(std::string group_name);
	std::string get_group_name() const;

	GroupMessage& add_member(int member_id);
	GroupMessage& remove_member(int member_id);
	bool is_member(int member_id) const;
	std::vector<int> get_member_list_ids() const;

	GroupMessage& add_admin(int admin_id);
	GroupMessage& remove_admin(int admin_id);
	bool is_admin(int admin_id) const;
	std::vector<int> get_admin_list_ids() const;

	GroupMessage& set_nickname(int user_id, std::string nickname);
	std::string get_nickname(int user_id) const;
	std::map<int, std::string> get_nickname_list() const;

	GroupMessage& add_pin_message(int message_id);
	GroupMessage& remove_pin_message(int message_id);
	std::vector<int> get_pin_message_list_ids() const;

	bool is_pin_message(int message_id) const;

	friend std::ostream& operator<<(std::ostream& out,
				const GroupMessage& group_message);

	static void Serialize(Walnut::StreamWriter* serializer,
				const GroupMessage& instance);

	static void Deserialize(Walnut::StreamReader* deserializer,
				GroupMessage& instance);

public:
	std::optional< Message > send_message(int sender_id, Message message) const;
	std::optional< Message> send_message(int sender_id, std::string message) const;

private:
	int m_group_id{};
	int m_conversation_id{};
	std::string m_group_name{};
	std::vector<int> m_member_list{};
	std::vector<int> m_admin_list{};
	std::map<int, std::string> m_nickname_list{};
	//std::string m_group_avatar{};
	std::vector<int> m_pin_message_list{};
	// images
	// attachments

};