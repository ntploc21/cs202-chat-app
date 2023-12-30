#pragma once

#include "Walnut/Serialization/StreamReader.h"
#include "Walnut/Serialization/StreamWriter.h"
#include "Walnut/Networking/Server.h"
#include "../ServerPacket.hpp"

class Response {
public:
    virtual void get_packet_type() const = 0;

    virtual void set_client_id(Walnut::ClientID client_id);
    virtual Walnut::ClientID get_client_id() const;

    virtual void set_session_id(int session);
    virtual int get_session_id() const;

protected:
    PacketType m_type{PacketType::None};
    Walnut::ClientID m_client_id{};
    int m_session_id{};
};