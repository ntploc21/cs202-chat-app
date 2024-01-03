#include "ServerHandlerFactory.hpp"

#include <iostream>

#include "AuthHandler.hpp"
#include "FriendHandler.hpp"

ServerHandlerFactory::~ServerHandlerFactory() {}

ServerHandler* ServerHandlerFactory::createHandler(Handler::ID id) {
    ServerHandler* handler = nullptr;
    switch (id) {
        case Handler::ID::Authenticate:
            handler = new AuthHandler();
            break;
        case Handler::ID::ClientRegister:
            handler = new ClientRegisterHandler();
            break;
        case Handler::ID::ClientLogin:
            handler = new ClientLoginHandler();
            break;
        case Handler::ID::ClientLogout:
            handler = new ClientLogoutHandler();
            break;
        case Handler::ID::RetrieveAllUsers:
            handler = new RetrieveAllUsersHandler();
            break;
        case Handler::ID::RetrieveAllFriends:
            handler = new RetrieveAllFriendsHandler();
            break;
        case Handler::ID::RetrieveAllGroups:
            handler = new RetrieveAllGroupsHandler();
            break;
        case Handler::ID::AddFriend:
            handler = new AddFriendHandler();
            break;
        case Handler::ID::AcceptFriend:
            handler = new AcceptFriendHandler();
            break;
        case Handler::ID::DeclineFriend:
            handler = new DeclineFriendHandler();
            break;
        case Handler::ID::Unfriend:
            handler = new UnfriendHandler();
            break;
        case Handler::ID::RetrievePendingFriendRequests:
            handler = new RetrievePendingFriendRequestsHandler();
            break;
    }
    return handler;
}

/*ServerHandler* ServerHandlerFactory::createHandler(
    const std::vector< Handler::ID >& ids,
    std::shared_ptr< Walnut::Server > m_server) {

    std::vector<std::unique_ptr<ServerHandler>> handlers;
    ServerHandler* handler = nullptr;

    for(auto id : ids) {
        ServerHandler* handler_tmp = createHandler(id);

        if(handler == nullptr) handler = handler_tmp;
        else handler->setNext(handler_tmp);

        handlers.push_back(std::unique_ptr< ServerHandler >(handler));
    }

    return handler;
}*/
