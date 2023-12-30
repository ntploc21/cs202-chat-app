#pragma once

#include "Session.hpp"
#include "Walnut/Networking/Server.h"
#include "Walnut/Serialization/BufferStream.h"

namespace Handler {
    enum class ID {
        Invalid,
        Authenticate,
        ClientRegister,
        ClientLogin,
        ClientLogout,

    };
};

class ServerHandler {
public:
    ServerHandler();

public:
    virtual void handle(const Walnut::ClientInfo& client_info,
                        Walnut::BufferStreamReader& stream);

    virtual ~ServerHandler() = default;

    void setNext(ServerHandler* p_next);

    void setServer(std::shared_ptr< Walnut::Server > server);

protected:
    virtual void handleImpl(const Walnut::ClientInfo& client_info,
                            Walnut::BufferStreamReader& stream) = 0;
    ServerHandler* m_next;
    std::shared_ptr< Walnut::Server > m_server;
};