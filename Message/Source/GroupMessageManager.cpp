#include "GroupMessageManager.hpp"

#include "ConversationManager.hpp"

GroupMessageManager::GroupMessageManager() { load_group_messages(); }

GroupMessageManager::~GroupMessageManager() {
    if (!m_used_by_client) save_group_messages();
}

int GroupMessageManager::add_group_message(GroupMessage group_message) {
    if (!m_used_by_client) group_message.set_group_id(++m_next_id);
    m_group_messages.push_back(group_message);
    if (!m_used_by_client) return m_next_id;
    return group_message.get_group_id();
}

int GroupMessageManager::new_group_message(std::vector< int > members,
                                           std::string group_name) {
    if (!m_used_by_client) return -1;
    int new_conv_id = ConversationManager::getInstance().new_conversation();
    GroupMessage new_group_message(++m_next_id, new_conv_id, group_name,
                                   members);

    m_group_messages.push_back(new_group_message);
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

std::optional< Message > GroupMessageManager::send_message(
    int sender_id, int group_id, std::string content) {
    if (m_used_by_client) return std::nullopt;

    int pos = findById(group_id);
    if (pos == -1) return std::nullopt;
    return m_group_messages[pos].send_message(sender_id, content);
}

void GroupMessageManager::set_used_by_client() {
    m_group_messages.clear();
    m_next_id = 0;
    m_used_by_client = true;
}

void GroupMessageManager::load_group_messages(
    std::vector< GroupMessage > group_messages) {
    if (!m_used_by_client) return;
    m_group_messages = group_messages;
    m_next_id = 0;
}

void GroupMessageManager::clear_data() {
    if(!m_used_by_client) return;
    m_group_messages.clear();
    m_next_id = 0;
}

void GroupMessageManager::save_group_messages() {
    // output to m_group_messages_file

    YAML::Emitter out;

    out << YAML::BeginSeq;

    for (auto group_message : m_group_messages) {
        out << group_message;
    }

    out << m_next_id;

    out << YAML::EndSeq;

    std::ofstream fout(m_group_messages_file.data());

    fout << out.c_str();

    fout.close();
}

void GroupMessageManager::load_group_messages() {
    // input from m_group_messages_file

    std::ifstream fin(m_group_messages_file.data());

    if (!fin.is_open()) {
        std::cout << "File not found: " << m_group_messages_file.data()
                  << std::endl;
        return;
    }

    YAML::Node doc = YAML::Load(fin);

    for (int i = 0; i < doc.size() - 1; i++) {
        GroupMessage group_message;
        doc[i] >> group_message;
        m_group_messages.push_back(group_message);
    }

    m_next_id = doc[doc.size() - 1].as< int >();
}

int GroupMessageManager::findById(int id) {
    for (int i = 0; i < m_group_messages.size(); i++) {
        if (m_group_messages[i].get_group_id() == id) {
            return i;
        }
    }
    return -1;
}