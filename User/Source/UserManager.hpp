#include "User.hpp"
#include "UserFilter.hpp"
#include <optional>

/*
User Manager roles
- Manage Users (add, remove, edit)
- Validate Users (check if user is valid, check credentials, etc)
- Store Users (save to file, load from file)
*/

class UserManager {
public:
    UserManager();
	~UserManager();

	int add_user( User user );
	bool delete_user(int user_id);
	bool update_user( User user );
	User get_user( int user_id );
	std::vector< User > get_users();

	std::vector< User > filter(
            std::unique_ptr< ISpecification< User > >& spec);

	std::optional<User> findByUsernameAndPassword( std::string username, std::string password );

	void save_users();
	void load_users();


private:
	int findById( int id );
	int findByUsername( std::string username );


private:
	std::vector< User > m_users{};
    int m_next_id { 0 };
};