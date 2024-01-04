#pragma once

#include "GroupMessage.hpp"

class GroupMessageManager {
public:
    GroupMessageManager(GroupMessageManager const&) = delete;
    void operator=(GroupMessageManager const&) = delete;
    static GroupMessageManager& getInstance() {
        static GroupMessageManager instance;  // Guaranteed to be destroyed.
                                      // Instantiated on first use.
        return instance;
    }

private:
    GroupMessageManager();
    ~GroupMessageManager();

public:
    int add_group_message(GroupMessage group_message);
    int new_group_message(std::vector< int > members, std::string group_name);
	bool delete_group_message(int group_message_id);
	bool update_group_message(GroupMessage group_message);
	GroupMessage get_group_message(int group_message_id);
	std::vector< GroupMessage > get_group_messages();

    std::optional< Message > send_message(int sender_id, int group_id, std::string content);

    void set_used_by_client();

    void load_group_messages(std::vector< GroupMessage > group_messages);


private:
	void save_group_messages();
	void load_group_messages();

	int findById(int id);

private:
    std::vector< GroupMessage > m_group_messages{};
	int m_next_id{};

    std::string_view m_group_messages_file{"group_messages.yaml"};

    bool m_used_by_client{false};

};