#include "UserFilter.hpp"

std::vector< User > UserFilter::filter(
    std::vector< User >& items,
    std::unique_ptr< Specification< User > >& spec) {
    std::vector< User > result;

    for (auto& item : items) {
        if (spec->isSatisfiedBy(item)) {
            result.push_back(item);
        }
    }

    return result;
}

UserOnlineSpecification::UserOnlineSpecification(bool p_online)
    : m_online(p_online) {}

bool UserOnlineSpecification::isSatisfiedBy(const User& user) const {
    return user.is_online() == m_online;
}

UserActiveSpecification::UserActiveSpecification(bool p_active)
    : m_active(p_active) {}

bool UserActiveSpecification::isSatisfiedBy(const User& user) const {
    return user.is_active() == m_active;
}