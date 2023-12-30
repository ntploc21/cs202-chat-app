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

Session& ClientManager::operator[](Walnut::ClientID id) {
    return m_clients[id];
}

Session ClientManager::operator[](Walnut::ClientID id) const {
    return m_clients.at(id);
}