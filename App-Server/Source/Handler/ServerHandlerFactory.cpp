#include "ServerHandlerFactory.hpp"
#include "AuthHandler.hpp"

ServerHandlerFactory::~ServerHandlerFactory() {}

ServerHandler* ServerHandlerFactory::createHandler(
    Handler::ID id) {
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
