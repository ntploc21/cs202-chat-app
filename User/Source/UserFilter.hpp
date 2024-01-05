#pragma once

#include "User.hpp"
#include "Filter.hpp"


class UserFilter : public IFilter< User > {
public:
    std::vector< User > filter(
        std::vector< User >& items,
        std::unique_ptr< ISpecification< User > >& spec) override;
};

/* User specifications */

class UserOnlineSpecification : public ISpecification< User > {
public:
    UserOnlineSpecification(bool p_online);
    virtual ~UserOnlineSpecification() = default;

    virtual bool isSatisfiedBy(const User& user) const override;

private:
    bool m_online{};
};

class UserActiveSpecification : public ISpecification< User > {
public:
    UserActiveSpecification(bool p_active);
    virtual ~UserActiveSpecification() = default;

    virtual bool isSatisfiedBy(const User& user) const override;

private:
    bool m_active{};
};