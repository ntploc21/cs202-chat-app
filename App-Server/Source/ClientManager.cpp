#include "ClientManager.hpp"

ClientManager& ClientManager::getInstance() {
    static ClientManager instance;  // Guaranteed to be destroyed.
                                    // Instantiated on first use.
    return instance;
}

ClientManager::ClientManager() {}

ClientManager::~ClientManager() {}

void ClientManager::addClient(Walnut::ClientID id) {
    m_clients.insert(std::pair< Walnut::ClientID, Session >(id, Session{}));
}

void ClientManager::addClient(Walnut::ClientID id, Session session) {
    m_clients.insert(std::pair< Walnut::ClientID, Session >(id, session));
}

void ClientManager::removeClient(Walnut::ClientID id) { m_clients.erase(id); }

std::vector< Walnut::ClientID > ClientManager::getAllClients() const {
    std::vector< Walnut::ClientID > clients;
    for (auto& client : m_clients) {
        clients.push_back(client.first);
    }
    return clients;
}

std::optional<Walnut::ClientID> ClientManager::getClientID(int user_id) const {
    for (auto& client : m_clients) {
        if (client.second.get_user_id() == user_id) return client.first;
    }
    return std::nullopt;
}

Session& ClientManager::operator[](Walnut::ClientID id) {
    return m_clients[id];
}

Session ClientManager::operator[](Walnut::ClientID id) const {
    return m_clients.at(id);
}