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
        case PacketType::AddFriend:
            return "PacketType::AddFriend";
        case PacketType::AcceptFriend:
            return "PacketType::AcceptFriend";
        case PacketType::DeclineFriend:
			return "PacketType::DeclineFriend";
        case PacketType::Unfriend:
			return "PacketType::Unfriend";
        case PacketType::RetrievePendingFriendRequests:
			return "PacketType::RetrievePendingFriendRequests";
        case PacketType::RetrieveAllGroups:
            return "PacketType::RetrieveAllGroups";
        case PacketType::RetrieveAllDMs:
            return "PacketType::RetrieveAllDMs";
        case PacketType::SendDirectMessage:
			return "PacketType::SendDirectMessage";
        case PacketType::SendGroupMessage:
            return "PacketType::SendGroupMessage";
    }

    return "PacketType::<Invalid>";
}
