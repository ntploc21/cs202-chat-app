#include "ServerController.hpp"

ServerController::ServerController() {}

ServerController::ServerController(std::shared_ptr< Walnut::Server > server)
	: m_server(server) {}