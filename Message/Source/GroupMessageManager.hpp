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
	bool delete_group_message(int group_message_id);
	bool update_group_message(GroupMessage group_message);
	GroupMessage get_group_message(int group_message_id);
	std::vector< GroupMessage > get_group_messages();

    std::optional< Message > send_message(int sender_id, int group_id, std::string content);

private:
	void save_group_messages();
	void load_group_messages();

	int findById(int id);

private:
    std::vector< GroupMessage > m_group_messages{};
	int m_next_id{};

};