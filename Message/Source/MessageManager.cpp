#include "MessageManager.hpp"

#include "UserManager.hpp"

MessageManager::MessageManager() { load_messages(); }

MessageManager::~MessageManager() {
    if (!m_used_by_client) save_messages();
}

int MessageManager::add_message(Message message) {
    if (!m_used_by_client) message.set_msg_id(++m_next_id);
    m_messages.push_back(message);
    if (!m_used_by_client) return m_next_id;
    return message.get_msg_id();
}

bool MessageManager::delete_message(int message_id) {
    int pos = findById(message_id);
    if (pos != -1) {
        m_messages.erase(m_messages.begin() + pos);
        return true;
    }
    return false;
}

bool MessageManager::update_message(Message message) {
    int pos = findById(message.get_msg_id());
    if (pos != -1) {
        m_messages[pos] = message;
        return true;
    }
    return false;
}

std::optional< Message > MessageManager::get_message(int message_id) {
    int pos = findById(message_id);
    if (pos == -1) return std::nullopt;
    return m_messages[pos];
}

std::vector< Message > MessageManager::get_messages() { return m_messages; }

std::optional< Message > MessageManager::send_message(int sender_id,
                                                      int receiver_id,
                                                      std::string content) {
    if (m_used_by_client) return std::nullopt;

    if (UserManager::getInstance().exists(sender_id) == false) {
        return std::nullopt;
    }

    Message message{++m_next_id, sender_id, receiver_id, content,
                    MessageType::Text};
    m_messages.push_back(message);
    return message;
}

void MessageManager::set_used_by_client() {
    m_messages.clear();
    m_next_id = 0;
    m_used_by_client = true;
}

void MessageManager::insert_messages(std::vector< Message > messages) {
    if(!m_used_by_client) return;
    m_messages.insert(m_messages.end(), messages.begin(), messages.end());
    m_next_id = 0;
}

void MessageManager::clear_data() {
    if(!m_used_by_client) return;
    m_messages.clear();
    m_next_id = 0;
}

void MessageManager::save_messages() {
    // output to m_messages_file

    YAML::Emitter out;

    out << YAML::BeginSeq;

    for (auto& message : m_messages) {
        out << message;
    }

    out << m_next_id;

    out << YAML::EndSeq;

    std::ofstream fout(m_messages_file.data());

    fout << out.c_str();

    fout.close();
}

void MessageManager::load_messages() {
    // input from m_users_file

    std::ifstream fin(m_messages_file.data());

    if (!fin.is_open()) {
        std::cout << "File not found!\n";
        return;
    }

    YAML::Node doc = YAML::Load(fin);

    for (int i = 0; i < doc.size() - 1; i++) {
        Message msg;
        doc[i] >> msg;
        m_messages.push_back(msg);
    }

    m_next_id = doc[doc.size() - 1].as< int >();

    std::cout << "hihi" << std::endl;
}

int MessageManager::findById(int id) {
    for (int i = 0; i < m_messages.size(); i++) {
        if (m_messages[i].get_msg_id() == id) {
            return i;
        }
    }
    return -1;
}