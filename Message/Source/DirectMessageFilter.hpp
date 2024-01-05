#pragma once

#include "DirectMessage.hpp"
#include "Filter.hpp"

class DirectMessageFilter : public IFilter< DirectMessage > {
public:
    std::vector< DirectMessage > filter(
        std::vector< DirectMessage >& items,
        std::unique_ptr< ISpecification< DirectMessage > >& spec) override;
};

/* User specifications */

class DMNameSpecification : public ISpecification< DirectMessage > {
public:
    DMNameSpecification(int user_id, std::string name);
    virtual ~DMNameSpecification() = default;

    virtual bool isSatisfiedBy(const DirectMessage& direct_message) const override;

private:
    int m_user_id{};
    std::string m_name{};
};

class DMLastMessageSpecification : public ISpecification< DirectMessage > {
public:
    DMLastMessageSpecification(std::string name);
    virtual ~DMLastMessageSpecification() = default;

    virtual bool isSatisfiedBy(
        const DirectMessage& direct_message) const override;

private:
    std::string m_name{};
};