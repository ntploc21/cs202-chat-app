#include "Session.hpp"

Session::Session(int session_id, int user_id)
    : m_session_id(session_id), m_user_id(user_id) {
    // expiration_date = date::today() + days(????);
}

void Session::set_session_id(int session_id) { m_session_id = session_id; }

int Session::get_session_id() const { return m_session_id; }

void Session::set_user_id(int user_id) { m_user_id = user_id; }

int Session::get_user_id() const { return m_user_id; }
