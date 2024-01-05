#include "DirectMessageManager.hpp"

#include "ConversationManager.hpp"

DirectMessageManager::DirectMessageManager() { load_direct_messages(); }

DirectMessageManager::~DirectMessageManager() {
    if (!m_used_by_client) save_direct_messages();
}

int DirectMessageManager::add_direct_message(DirectMessage direct_message) {
    if (!m_used_by_client) direct_message.set_dm_id(++m_next_id);
    m_direct_messages.push_back(direct_message);
    if (!m_used_by_client) return m_next_id;
    return direct_message.get_dm_id();
}

int DirectMessageManager::new_direct_message(int user_1_id, int user_2_id) {
    if (m_used_by_client) return -1;

    int new_conv_id = ConversationManager::getInstance().new_conversation();
    DirectMessage new_dm(++m_next_id, new_conv_id, user_1_id, user_2_id);
    m_direct_messages.push_back(new_dm);
    return m_next_id;
}

bool DirectMessageManager::add_message(int direct_message_id, Message message) {
    int pos = findById(direct_message_id);
    if (pos == -1) return false;
    return ConversationManager::getInstance().add_message(
        m_direct_messages[pos].get_conversation_id(), message);
}

bool DirectMessageManager::delete_message(int direct_message_id,
                                          int message_id) {
    int pos = findById(direct_message_id);
    if (pos == -1) return false;
    return ConversationManager::getInstance().delete_message(
        m_direct_messages[pos].get_conversation_id(), message_id);
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

std::optional< DirectMessage > DirectMessageManager::get_direct_message(
    int direct_message_id) {
    int pos = findById(direct_message_id);
    if (pos == -1) return std::nullopt;
    return m_direct_messages[pos];
}

std::optional< DirectMessage > DirectMessageManager::get_direct_message(
    int user_1_id, int user_2_id) {
    for (auto& dm : m_direct_messages) {
        if ((dm.get_user_id_1() == user_1_id &&
             dm.get_user_id_2() == user_2_id) ||
            (dm.get_user_id_1() == user_2_id &&
             dm.get_user_id_2() == user_1_id)) {
            return dm;
        }
    }
    return std::nullopt;
}

std::vector< DirectMessage > DirectMessageManager::get_direct_messages() {
    return m_direct_messages;
}

std::vector< DirectMessage > DirectMessageManager::get_direct_messages(
    int user_id) {
    std::vector< DirectMessage > result;
    for (auto& dm : m_direct_messages) {
        if (dm.get_user_id_1() == user_id || dm.get_user_id_2() == user_id) {
            result.push_back(dm);
        }
    }
    return result;
}

std::optional< Message > DirectMessageManager::send_message(
    int sender_id, int receiver_id, std::string content) {
    if (m_used_by_client) return std::nullopt;

    int pos = findById(receiver_id);
    if (pos == -1) return std::nullopt;
    return m_direct_messages[pos].send_message(sender_id, content);
}

std::optional< Message > DirectMessageManager::send_announcement(
    int direct_message_id, int sender_id, std::string content) {
    if (m_used_by_client) return std::nullopt;

    int pos = findById(direct_message_id);
    if (pos == -1) return std::nullopt;
    return m_direct_messages[pos].send_announcement(sender_id, content);
}

void DirectMessageManager::set_used_by_client() {
    m_direct_messages.clear();
    m_used_by_client = true;
    m_next_id = 0;
}

void DirectMessageManager::load_direct_messages(
    std::vector< DirectMessage > direct_messages) {
    if (!m_used_by_client) return;

    m_direct_messages = direct_messages;
    m_next_id = 0;
}

bool DirectMessageManager::update_last_seen(int direct_message_id, int user_id,
                                            date::sys_seconds last_seen_at) {
    int pos = findById(direct_message_id);
    if (pos == -1) return false;

    m_direct_messages[pos].set_user_last_seen_at(user_id, last_seen_at);

    return true;
}

bool DirectMessageManager::add_pin_message(int direct_message_id,
                                           int message_id) {
    int pos = findById(direct_message_id);
    if (pos == -1) return false;

    // check if message_id is in conversation
    auto conv = ConversationManager::getInstance().get_conversation(
        m_direct_messages[pos].get_conversation_id());
    if (!conv.has_value()) return false;

    auto messages = conv.value().get_messages();
    bool found = false;

    // check if message_id is in the pin list
    for (int pin_id : m_direct_messages[pos].m_pin_message_list) {
        if (pin_id == message_id) {
            found = true;
            break;
        }
    }

    if (found) return false;

    // add message_id to pin list
    m_direct_messages[pos].add_pin_message(message_id);
}

bool DirectMessageManager::remove_pin_message(int direct_message_id,
                                              int message_id) {
    int pos = findById(direct_message_id);
    if (pos == -1) return false;

    // check if message_id is in the pin list
    bool found = false;

    for (int pin_id : m_direct_messages[pos].m_pin_message_list) {
        if (pin_id == message_id) {
            found = true;
            break;
        }
    }

    if (!found) return false;
    // remove message_id from pin list
    m_direct_messages[pos].remove_pin_message(message_id);

    return true;
}

bool DirectMessageManager::update_nickname(int direct_message_id, int user_id,
                                           std::string nickname) {
    int pos = findById(direct_message_id);
    if (pos == -1) return false;

    m_direct_messages[pos].set_user_nickname(user_id, nickname);

    return true;
}

void DirectMessageManager::clear_data() {
    if (!m_used_by_client) return;
    m_direct_messages.clear();
    m_next_id = 0;
}

void DirectMessageManager::save_direct_messages() {
    // output to m_direct_messages_file

    YAML::Emitter out;

    out << YAML::BeginSeq;

    for (auto& dm : m_direct_messages) {
        out << dm;
    }

    out << m_next_id;

    out << YAML::EndSeq;

    std::ofstream fout(m_direct_messages_file.data());

    fout << out.c_str();

    fout.close();
}

void DirectMessageManager::load_direct_messages() {
    // input from m_direct_messages_file

    std::ifstream fin(m_direct_messages_file.data());

    if (!fin.is_open()) {
        std::cout << "File not found: " << m_direct_messages_file.data()
                  << std::endl;
        return;
    }

    YAML::Node doc = YAML::Load(fin);

    for (int i = 0; i < doc.size() - 1; i++) {
        DirectMessage dm;
        doc[i] >> dm;
        m_direct_messages.push_back(dm);
    }

    m_next_id = doc[doc.size() - 1].as< int >();
}

int DirectMessageManager::findById(int id) {
    for (int i = 0; i < m_direct_messages.size(); i++) {
        if (m_direct_messages[i].get_dm_id() == id) {
            return i;
        }
    }
    return -1;
}
