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

	/* -- ClientDisconnect --
		
	
	*/
	ClientDisconnect = 4,

	/* -- ServerShutdown --
		[Server -> Client]
	*/
	ServerShutdown = 5,

};

std::string_view PacketTypeToString(PacketType type);