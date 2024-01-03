#include "User-Utils.hpp"

void filter_credentials(User& user) {
	user.set_password("");
}

void filter_credentials(std::vector<User>& users) {
	for (auto& user : users) {
		filter_credentials(user);
	}
}