#pragma once

class Session {
public:
    Session(int session_id, int user_id);
    void set_session_id(int session_id);
    int get_session_id() const;

    void set_user_id(int user_id);
    int get_user_id() const;

    // date get_expiration_date() const { return expiration_date; }
    // void set_expiration_date(date expiration_date) { this->expiration_date =
    // expiration_date;

private:
    int m_session_id{};
    int m_user_id{};
    // date expiration_date;
};