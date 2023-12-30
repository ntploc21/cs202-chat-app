#include "ServerHandler.hpp"

ServerHandler::ServerHandler() : m_next(nullptr), m_server(nullptr) {}

void ServerHandler::handle(const Walnut::ClientInfo& client_info,
                           Walnut::BufferStreamReader& stream) {
    handleImpl(client_info, stream);
    if (m_next) m_next->handle(client_info, stream);
}

void ServerHandler::setNext(ServerHandler* next) {
    if (m_next)
        m_next->setNext(next);
    else
        m_next = next;
}

void ServerHandler::setServer(std::shared_ptr< Walnut::Server > server) {
    m_server = server;
}