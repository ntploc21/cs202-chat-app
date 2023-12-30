#pragma once
#include "ServerHandler.hpp"

class ServerHandlerFactory {
public:
    ServerHandlerFactory(ServerHandlerFactory const&) = delete;
    void operator=(ServerHandlerFactory const&) = delete;
    static ServerHandlerFactory& getInstance() {
        static ServerHandlerFactory instance;  // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return instance;
    }

private:
    ServerHandlerFactory();
    ~ServerHandlerFactory();

public:
    static ServerHandler* createHandler(Handler::ID id);
    //ServerHandler* createHandler(const std::vector<Handler::ID>& ids, std::shared_ptr<Walnut::Server> m_server);
};