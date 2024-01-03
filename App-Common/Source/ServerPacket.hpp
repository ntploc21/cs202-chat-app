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
		1. std::vector< User >
	*/
	RetrieveAllUsers = 4,

	/* -- RetrieveAllFriends --
		[Client -> Server]
		
		[Server -> Client]
		1. std::vector< User >
	*/
	RetrieveAllFriends = 5,

	/* -- RetrieveAllGroups --
		[Client -> Server]

		[Server -> Client]
		std::vector<GroupMessage>
	
	*/
	RetrieveAllGroups = 6,

	/* -- AddFriend --
		[Client -> Server]
		1. int friend_id
		2. std::string note

		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	AddFriend = 7,

	/* -- AcceptFriend --
		[Client -> Server]
		1. int user_id
	 
		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	AcceptFriend = 8,

	/* -- DeclineFriend --
		[Client -> Server]
		1. int user_id
	
		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	DeclineFriend = 9,

	/* -- Unfriend --
		[Client -> Server]
		1. int user_id

		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
	*/
	Unfriend = 10,

	/* -- RetrievePendingFriendRequests
		[Client -> Server]

		[Server -> Client]
		1. bool success
		2. std::vector< User >
		3. std::vector< std::string >
	*/
	RetrievePendingFriendRequests = 11,

};

std::string_view PacketTypeToString(PacketType type);