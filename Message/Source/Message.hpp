#pragma once

#include <stdint.h>
#include "Walnut/Serialization/StreamReader.h"
#include "Walnut/Serialization/StreamWriter.h"

enum class MessageType : uint16_t {
    None = 0, // invalid
    Text = 1,
    Image = 2,
    File = 3,
    Announcement = 5, // only the server / (may be admin?) itself can send this
};

class Message {
public:
    Message();
    Message(int msg_id, int from_id, int to_conversation_id, std::string content, MessageType type, bool forward = 0, bool active = 1);
    virtual ~Message() = default;

    int get_msg_id() const;
    int get_from_id() const;
    int get_to_conversation_id() const;
    std::string get_content() const;
    MessageType get_type() const;
    bool is_forwarded() const;
    bool is_active() const;

    Message& set_msg_id(int msg_id);
    Message& set_from_id(int from_id);
    Message& set_to_conversation_id(int to_conversation_id);
    Message& set_content(std::string content);
    Message& set_type(MessageType type);
    Message& set_forward(bool forward);
    Message& set_active(bool active);

    
    friend std::ostream& operator<<(std::ostream& out, const Message& user);

    static void Serialize(Walnut::StreamWriter* serializer,
                          const Message& instance);

    static void Deserialize(Walnut::StreamReader* deserializer,
                            Message& instance);

public:
    int m_msg_id{};
    int m_from_id{}; // foreign key from user
    int m_to_conversation_id{}; // foreign key from conversation
    std::string m_content{};
    // date sent_date
    MessageType m_type{};
    bool m_forward {};
    bool m_active {};

};

std::string_view MessageTypeToString(MessageType type);