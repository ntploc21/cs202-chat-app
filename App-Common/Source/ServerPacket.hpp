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

    /* -- UpdateLastSeenDM --
            [Client -> Server]
            1. int dm_id

            [Server -> Client]
            1. bool success
            2. std::string error_msg (if not success)
            3. int dm_id
            4. int user_id
            5. date::sys_seconds last_seen_at
    */
    UpdateLastSeenDM = 15,

    /* -- AddPinMessageDM --
            [Client -> Server]
            1. int dm_id
            2. int message_id

            [Server -> Client]
            1. bool success
            2. std::string error_msg (if not success)
            3. int dm_id
            4. int message_id
    */
    AddPinMessageDM = 16,

    /* -- RemovePinMessageDM --
            [Client -> Server]
            1. int dm_id
            2. int message_id

            [Server -> Client]
            1. bool success
            2. std::string error_msg (if not success)
            3. int dm_id
            4. int message_id
    */
    RemovePinMessageDM = 17,

    /* -- AddPinMessageGroup --
            [Client -> Server]
            1. int group_id
            2. int message_id

            [Server -> Client]
            1. bool success
            2. std::string error_msg (if not success)
            3. int group_id
            4. int message_id

    */
    AddPinMessageGroup = 18,

    /* -- RemovePinMessageGroup --
            [Client -> Server]
            1. int group_id
            2. int message_id

            [Server -> Client]
            1. bool success
            2. std::string error_msg (if not success)
            3. int group_id
            4. int message_id
    */
    RemovePinMessageGroup = 19,

    /* -- DeleteMessageDM --
        [Client -> Server]
        1. int dm_id
        2. int message_id

        [Server -> Client]
        1. bool success
        2. std::string error_msg (if not success)
        3. int dm_id
        4. int message_id
    */
    DeleteMessageDM = 20,

    /* -- DeleteMessageGroup --
        [Client -> Server]
        1. int group_id
        2. int message_id

        [Server -> Client]
        1. bool success
        2. std::string error_msg (if not success)
        3. int group_id
        4. int message_id
    */
    DeleteMessageGroup = 21,

    /* -- UpdateNicknameDM --
        [Client -> Server]
        1. int dm_id
        2. int user_id
        3. std::string nickname

        [Server -> Client]
        1. bool success
        2. std::string error_msg (if not success)
        3. int dm_id
        4. int user_id
        5. std::string nickname
    */
    UpdateNicknameDM = 22,

    /* -- UpdateNicknameGroup --
		[Client -> Server]
		1. int group_id
		2. int user_id
		3. std::string nickname

		[Server -> Client]
		1. bool success
		2. std::string error_msg (if not success)
		3. int group_id
		4. int user_id
		5. std::string nickname
	*/
    UpdateNicknameGroup = 23,

    /* -- CreateGroup --
			[Client -> Server]
			1. std::string name
			3. std::vector<int> user_ids

			[Server -> Client]
			1. bool success
			2. std::string error_msg (if not success)
			3. Group group
	*/
    CreateGroup = 24,
};

std::string_view PacketTypeToString(PacketType type);