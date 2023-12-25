#pragma once

#include "Walnut/Serialization/StreamReader.h"
#include "Walnut/Serialization/StreamWriter.h"

class Session {
public:
    Session();
    Session(int session_id, int user_id);
    
    void set_session_id(int session_id);
    int get_session_id() const;

    void set_user_id(int user_id);
    int get_user_id() const;

    friend std::ostream& operator<<(std::ostream& out, const Session& user);

    static void Serialize(Walnut::StreamWriter* serializer,
                          const Session& instance);

    static void Deserialize(Walnut::StreamReader* deserializer,
                            Session& instance);

    // date get_expiration_date() const { return expiration_date; }
    // void set_expiration_date(date expiration_date) { this->expiration_date =
    // expiration_date;

    int m_session_id{};
    int m_user_id{};
    // date expiration_date;
};