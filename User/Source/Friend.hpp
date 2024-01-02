#pragma once

#include <string>
#include <vector>
#include "Walnut/Serialization/StreamReader.h"
#include "Walnut/Serialization/StreamWriter.h"

#include "yaml-cpp/yaml.h"

#include "date/date.h"

#include <chrono>

class Friend {
public:
	// make it as a builder using builder design pattern
	Friend();
	Friend(int friend_id, int user_1_id, int user_2_id, int user_1_status, int user_2_status, std::string note, int conversation_id);

	virtual ~Friend() = default;

	int get_friend_id() const;
	int get_user_1_id() const;
	int get_user_2_id() const;
	int get_user_1_status() const;
	int get_user_2_status() const;
	std::string get_note() const;
	int get_conversation_id() const;
	date::sys_seconds get_created_at() const;

	Friend& set_friend_id(int friend_id);
	Friend& set_user_1_id(int user_1_id);
	Friend& set_user_2_id(int user_2_id);
	Friend& set_user_1_status(int user_1_status);
	Friend& set_user_2_status(int user_2_status);
	Friend& set_note(std::string note = "");
	Friend& set_conversation_id(int conversation_id);
Friend& set_created_at(date::sys_seconds created_at);

	friend std::ostream& operator<<(std::ostream& out, const Friend& friend_);

	static void Serialize(Walnut::StreamWriter* serializer,
				const Friend& instance);

	static void Deserialize(Walnut::StreamReader* deserializer,
				Friend& instance);

	friend YAML::Emitter& operator<<(YAML::Emitter& out, const Friend& friend_);

	friend void operator>>(const YAML::Node& in, Friend& friend_);

public:
	int m_friend_id{};
	int m_user_1_id{};
	int m_user_2_id{};
	
	/*
		if user_1_status = 1 && user_2_status = 1
		=> friend

		if user_1_status = 1 && user_2_status = 0
		=> user_1 sent friend request to user_2

		if user_1_status = 0 && user_2_status = 1
		=> user_2 sent friend request to user_1
	*/
	int m_user_1_status{};
	int m_user_2_status{};

	std::string m_note{};

	int m_conversation_id{}; // ref DirectMessage

	date::sys_seconds m_created_at{};
};