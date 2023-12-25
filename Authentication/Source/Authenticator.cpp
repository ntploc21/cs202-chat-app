#include "Authenticator.hpp"
#include "UserManager.hpp"

Authenticator::Authenticator() {
    load_sessions();
}

Authenticator::~Authenticator() {
    save_sessions();
}

std::optional< std::pair< User, Session > > Authenticator::authenticate(
    std::string_view username, std::string_view password) {
    auto validate_result = UserManager::getInstance().findByUsernameAndPassword(
        username, password);

    if (!validate_result.has_value()) return std::nullopt;

    User user = validate_result.value();

    auto prev_session = find_session_pos_by_user_id(user.get_user_id());
    if (prev_session.has_value()) {
        m_sessions.erase(m_sessions.begin() + prev_session.value());
    }

    Session session(m_next_id++, user.get_user_id());
    m_sessions.push_back(session);

    return std::make_pair(user, session);
}

std::optional< User > Authenticator::validate_session(int session_id) {
    for (Session session : m_sessions) {
        if (session.get_session_id() == session_id) {
            /*
                remember to check if session is expired later
            */

            return UserManager::getInstance().get_user(session.get_user_id());
        }
    }

    return std::nullopt;
}

std::optional< Session > Authenticator::get_session(int session_id) const {
    for (Session session : m_sessions) {
        if (session.get_session_id() == session_id) {
            return session;
        }
    }

    return std::nullopt;
}

void Authenticator::remove_session(int session_id) {
    for (int i = 0; i < m_sessions.size(); ++i) {
        if (m_sessions[i].get_session_id() == session_id) {
            m_sessions.erase(m_sessions.begin() + i);
            return;
        }
    }
}

Session Authenticator::add_session(int user_id) {
    auto result = find_session_pos_by_user_id(user_id);
    if (result.has_value()) {
        m_sessions.erase(m_sessions.begin() + result.value());
    }

    Session session(m_next_id++, user_id);
    m_sessions.push_back(session);

    return session;
}

std::optional< int > Authenticator::find_session_pos_by_user_id(int user_id) {
    for (int i = 0; i < m_sessions.size(); ++i) {
        if (m_sessions[i].get_user_id() == user_id) {
            return i;
        }
    }

    return std::nullopt;
}

void Authenticator::save_sessions() { /* Implement later */
}

void Authenticator::load_sessions() { /* Implement later */
}