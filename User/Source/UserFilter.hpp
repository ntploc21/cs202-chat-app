#pragma once

#include "User.hpp"

/* Use specification design pattern to implement filter feature */
template< typename T >
class ANDSpecification;
template< typename T >
class ORSpecification;
template< typename T >
class NOTSpecification;

template< typename T >
class ISpecification {
public:
    virtual bool isSatisfiedBy(const T&) const = 0;

    std::unique_ptr< ISpecification< T > > operator&&(
        ISpecification< T >&& other) const {
        return std::make_unique< ANDSpecification< T > >(*this, other);
    }

    std::unique_ptr< ISpecification< T > > operator||(
        ISpecification< T >&& other) const {
        return std::make_unique< ORSpecification< T > >(*this, other);
    }

    std::unique_ptr< ISpecification< T > > operator!() const {
        return std::make_unique< NOTSpecification< T > >(*this);
    }
};

template< typename T >
class Filter {
public:
    virtual std::vector< T > filter(
        std::vector< T >& items,
        std::unique_ptr< ISpecification< T > >& spec) = 0;
};

class UserFilter : public Filter< User > {
public:
    std::vector< User > filter(
        std::vector< User >& items,
        std::unique_ptr< ISpecification< User > >& spec) override;
};

/* Specification business logics */

template< typename T >
class ANDSpecification : public ISpecification< T > {
public:
    ANDSpecification(ISpecification< T >& p_left, ISpecification< T >& p_right)
        : m_leftSpec(p_left), m_rightSpec(p_right) {}
    virtual ~ANDSpecification() = default;

    virtual bool isSatisfiedBy(const T& item) const override {
        return m_leftSpec.isSatisfiedBy(item) &&
               m_rightSpec.isSatisfiedBy(item);
    }

private:
    ISpecification< T >& m_leftSpec;
    ISpecification< T >& m_rightSpec;
};

template< typename T >
class ORSpecification : public ISpecification< T > {
public:
    ORSpecification(ISpecification< T >& p_left, ISpecification< T >& p_right)
        : m_leftSpec(p_left), m_rightSpec(p_right) {}
    virtual ~ORSpecification() = default;

    virtual bool isSatisfiedBy(const T& item) const override {
        return m_leftSpec.isSatisfiedBy(item) ||
               m_rightSpec.isSatisfiedBy(item);
    }

private:
    ISpecification< T >& m_leftSpec;
    ISpecification< T >& m_rightSpec;
};

template< typename T >
class NOTSpecification : public ISpecification< T > {
public:
    NOTSpecification(ISpecification< T >& p_cnd) : m_Spec(p_cnd) {}
    virtual ~NOTSpecification() = default;

    virtual bool isSatisfiedBy(const T& item) const override {
        return !m_Spec.isSatisfiedBy(item);
    }

private:
    ISpecification< T >& m_Spec;
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