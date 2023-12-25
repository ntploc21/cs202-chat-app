#pragma once
#include "ServerPacket.hpp"

std::string_view PacketTypeToString(PacketType type) {
    switch (type) {
        case PacketType::None:
            return "PacketType::None";
        case PacketType::ClientRegisterRequest:
            return "PacketType::ClientRegisterRequest";
        case PacketType::ClientLoginRequest:
            return "PacketType::ClientLoginRequest";
    }

    return "PacketType::<Invalid>";
}
