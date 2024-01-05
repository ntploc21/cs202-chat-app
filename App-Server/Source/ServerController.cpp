#include "ServerController.hpp"

#include <iostream>

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
    m_packet_handlers[PacketType::RetrieveAllUsers] = {
        Handler::ID::Authenticate, Handler::ID::RetrieveAllUsers};
    m_packet_handlers[PacketType::RetrieveAllFriends] = {
        Handler::ID::Authenticate, Handler::ID::RetrieveAllFriends};
    m_packet_handlers[PacketType::AddFriend] = {Handler::ID::Authenticate,
                                                Handler::ID::AddFriend};
    m_packet_handlers[PacketType::AcceptFriend] = {Handler::ID::Authenticate,
                                                   Handler::ID::AcceptFriend};
    m_packet_handlers[PacketType::DeclineFriend] = {Handler::ID::Authenticate,
                                                    Handler::ID::DeclineFriend};
    m_packet_handlers[PacketType::Unfriend] = {Handler::ID::Authenticate,
                                               Handler::ID::Unfriend};
    m_packet_handlers[PacketType::RetrievePendingFriendRequests] = {
        Handler::ID::Authenticate, Handler::ID::RetrievePendingFriendRequests};
    m_packet_handlers[PacketType::RetrieveAllGroups] = {
        Handler::ID::Authenticate, Handler::ID::RetrieveAllGroups};
    m_packet_handlers[PacketType::RetrieveAllDMs] = {
        Handler::ID::Authenticate, Handler::ID::RetrieveAllDMs};
    m_packet_handlers[PacketType::SendDirectMessage] = {
        Handler::ID::Authenticate, Handler::ID::SendDirectMessage};
    m_packet_handlers[PacketType::SendGroupMessage] = {
        Handler::ID::Authenticate, Handler::ID::SendGroupMessage};
    m_packet_handlers[PacketType::UpdateLastSeenDM] = {
        Handler::ID::Authenticate, Handler::ID::UpdateLastSeenDM};
    m_packet_handlers[PacketType::AddPinMessageDM] = {
        Handler::ID::Authenticate, Handler::ID::AddPinMessageDM};
    m_packet_handlers[PacketType::RemovePinMessageDM] = {
        Handler::ID::Authenticate, Handler::ID::RemovePinMessageDM};
    m_packet_handlers[PacketType::AddPinMessageGroup] = {
        Handler::ID::Authenticate, Handler::ID::AddPinMessageGroup};
    m_packet_handlers[PacketType::RemovePinMessageGroup] = {
        Handler::ID::Authenticate, Handler::ID::RemovePinMessageGroup};
    m_packet_handlers[PacketType::DeleteMessageDM] = {
        Handler::ID::Authenticate, Handler::ID::DeleteMessageDM};
    m_packet_handlers[PacketType::DeleteMessageGroup] = {
        Handler::ID::Authenticate, Handler::ID::DeleteMessageGroup};
    m_packet_handlers[PacketType::UpdateNicknameDM] = {
        Handler::ID::Authenticate, Handler::ID::UpdateNicknameDM};
    m_packet_handlers[PacketType::UpdateNicknameGroup] = {
        Handler::ID::Authenticate, Handler::ID::UpdateNicknameGroup};
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