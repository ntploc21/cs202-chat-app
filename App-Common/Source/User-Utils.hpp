#pragma once

#include <string_view>

#include "User.hpp"

void filter_credentials(User& user);

void filter_credentials(std::vector< User >& users);