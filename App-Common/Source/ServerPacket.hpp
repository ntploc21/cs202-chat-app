#pragma once

#include <stdint.h>
#include <string_view>

/* Common "protocol" for server <-> client communication */

enum class PacketType : uint16_t {
	/* Invalid request */
	None = 0,

	/* -- ClientRegisterRequest --
		[Client -> Server]
		1. std::string username
		2. std::string password
		3. std::string fullname

		[Server -> Client]
		1. bool success
		2. Session object (if success)
		3. User object (if success)
		4. std::string error_msg (if not success)
	
	*/
	ClientRegisterRequest = 1,
	
	/* -- ClientLoginRequest --
		[Client -> Server]
		1. std::string username
		2. std::string password

		[Server -> Client]
		1. bool success
		2. Session object (if success)
		3. User object (if success)
	*/
	ClientLoginRequest = 2,

	/* -- ClientLoginRequestWithToken --
		[Client -> Server]
		1. std::string session_token

		[Server -> Client]
		1. bool success
		2. Session object (if success)
		3. User object (if success)
	*/

	/* -- ClientLogoutRequest --
		[Client -> Server]
		
		[Server -> Client]
		1. bool success

	*/
	ClientLogoutRequest = 3,

	/* -- RetrieveAllUsers --
		[Client -> Server]

		[Server -> Client]
		1. success
		2. std::vector< User >
	*/
	RetrieveAllUsers = 4,

	/* -- RetrieveAllFriends --
		[Client -> Server]
		
		[Server -> Client]
		1. success
		2. std::vector< User >
	*/
	RetrieveAllFriends = 5,

	/* -- AddFriend --
		[Client -> Server]
		1. int friend_id
		2. std::string note

		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	AddFriend = 6,

	/* -- AcceptFriend --
		[Client -> Server]
		1. int user_id
	 
		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	AcceptFriend = 7,

	/* -- DeclineFriend --
		[Client -> Server]
		1. int user_id
	
		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	DeclineFriend = 8,

	/* -- Unfriend --
		[Client -> Server]
		1. int user_id

		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	Unfriend = 9,

	/* -- RetrievePendingFriendRequests --
		[Client -> Server]

		[Server -> Client]
		1. bool success
		2. std::vector< User >
		3. std::vector< std::string >
	*/
    RetrievePendingFriendRequests = 10,

	/* -- RetrieveAllDirectMessages --
		[Client -> Server]
		1. int dm_id

		[Server -> Client]
		1. bool success
		2. vector<DirectMessage>
		3. vector<Conversation>
		4. vector<Message>
	*/
    RetrieveAllDMs = 11,

    /* -- RetrieveAllGroups --
        [Client -> Server]

        [Server -> Client]
        1. success
        2. std::vector<GroupMessage>
        3. std::vector<Conversation>
        4. std::vector<Message>
        */
    RetrieveAllGroups = 12,

	/* -- SendDirectMessage --
		[Client -> Server]
		1. int dm_id
		2. std::string message
		
		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
		3. int dm_id
		4. Message msg
	*/
	SendDirectMessage = 13,

	/* -- SendGroupMessage --
		[Client -> Server]
		1. int group_id
		2. std::string message
							
		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	SendGroupMessage = 14,

};

std::string_view PacketTypeToString(PacketType type);