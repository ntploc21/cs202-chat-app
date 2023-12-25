#pragma once

#include "Session.hpp"
#include "User.hpp"
#include <optional>
#include <string_view>

class Authenticator {
public:
    Authenticator(Authenticator const&) = delete;
    void operator=(Authenticator const&) = delete;
    static Authenticator& getInstance() {
        static Authenticator instance;  // Guaranteed to be destroyed.
                                      // Instantiated on first use.
        return instance;
    }

private:
    Authenticator();
    ~Authenticator();


public:
    std::optional< std::pair< User, Session > > authenticate(
        std::string_view username, std::string_view password);

    // return user id associate with the session
    std::optional< User > validate_session(int session_id);

    std::optional< Session > get_session(int session_id) const;
    void remove_session(int session_id);
    Session add_session(int user_id);

private:
    std::optional< int > find_session_pos_by_user_id(int user_id);

    void save_sessions();
    void load_sessions();

private:
    std::vector< Session > m_sessions;

    int m_next_id{0};
};