#include "Session.hpp"
#include "UserManager.hpp"

class Authenticator {
public:
    Authenticator(std::shared_ptr< UserManager > user_manager);
    std::optional< std::pair<User, Session> > authenticate(std::string_view username,
                                          std::string_view password);
    
    // return user id associate with the session
    std::optional< User > validate_session(int session_id);

    std::optional< Session > get_session(int session_id) const;
    void remove_session(int session_id);

private:
    std::optional<int> find_session_pos_by_user_id(int user_id);

    void save_sessions();
    void load_sessions();

private:
    std::vector< Session > m_sessions;
    std::shared_ptr< UserManager > m_user_manager;

    int m_next_id{0};
};