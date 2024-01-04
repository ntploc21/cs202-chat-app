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
        case PacketType::ClientLogoutRequest:
            return "PacketType::ClientLogoutRequest";
        case PacketType::RetrieveAllUsers:
            return "PacketType::RetrieveAllUsers";
        case PacketType::RetrieveAllFriends:
            return "PacketType::RetrieveAllFriends";
        case PacketType::RetrievePendingFriendRequests:
			return "PacketType::RetrievePendingFriendRequests";
        case PacketType::RetrieveAllGroups:
            return "PacketType::RetrieveAllGroups";
        case PacketType::RetrieveAllDMs:
            return "PacketType::RetrieveAllDMs";
    }

    return "PacketType::<Invalid>";
}
