#pragma once
#include "Walnut/Networking/Server.h"
#include "Session.hpp"

class ClientManager {
public:
    ClientManager(ClientManager const&) = delete;
    void operator=(ClientManager const&) = delete;
    static ClientManager& getInstance();

private:
    ClientManager();
    ~ClientManager();

public:
    void addClient(Walnut::ClientID id);
    void addClient(Walnut::ClientID id, Session session);
    void removeClient(Walnut::ClientID id);

    Session& operator[](Walnut::ClientID id);
    Session operator[](Walnut::ClientID id) const;

private:
    std::map<Walnut::ClientID, Session> m_clients;
};