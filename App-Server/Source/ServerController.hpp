#pragma once
#include "Walnut/Networking/Server.h"

class ServerController {
public:
    ServerController();
    ServerController(std::shared_ptr< Walnut::Server > server);

private:
    std::shared_ptr< Walnut::Server > m_server{};

};