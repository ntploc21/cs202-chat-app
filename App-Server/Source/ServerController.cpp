#include "ServerController.hpp"

#include "Handler/ServerHandlerFactory.hpp"
#include "Walnut/Core/Assert.h"
#include "Walnut/Serialization/BufferStream.h"

ServerController::ServerController() { constructHandlers(); }

ServerController::ServerController(std::shared_ptr< Walnut::Server > server)
    : m_server(server) {
    constructHandlers();
}

void ServerController::constructHandlers() {
    m_packet_handlers[PacketType::ClientLoginRequest] = {
        Handler::ID::ClientLogin};
    m_packet_handlers[PacketType::ClientRegisterRequest] = {
        Handler::ID::ClientRegister};
    m_packet_handlers[PacketType::ClientLogoutRequest] = {
        Handler::ID::Authenticate, Handler::ID::ClientLogout};
}

void ServerController::handlePacket(const Walnut::ClientInfo& client_info,
                                    const Walnut::Buffer buffer) {
    Walnut::BufferStreamReader stream(buffer);

    PacketType type;
    bool success = stream.ReadRaw< PacketType >(type);
    WL_CORE_VERIFY(success);

    executeHandlers(client_info, type, stream);
}

void ServerController::executeHandlers(const Walnut::ClientInfo& client_info,
                                       const PacketType packet_type,
                                       Walnut::BufferStreamReader& stream) {
    auto ids = m_packet_handlers[packet_type];
    std::vector< std::unique_ptr< ServerHandler > > handlers;
    ServerHandler* handler = nullptr;

    for (auto id : ids) {
        ServerHandler* handler_tmp = ServerHandlerFactory::createHandler(id);

        handler_tmp->setServer(m_server);

        if (handler == nullptr)
            handler = handler_tmp;
        else
            handler->setNext(handler_tmp);

        handlers.push_back(std::unique_ptr< ServerHandler >(handler_tmp));
    }

    if (handler != nullptr) handler->handle(client_info, packet_type, stream);
}