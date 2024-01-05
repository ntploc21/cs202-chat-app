#include "DirectMessageFilter.hpp"

#include "ConversationManager.hpp"

std::vector< DirectMessage > DirectMessageFilter::filter(
    std::vector< DirectMessage >& items,
    std::unique_ptr< ISpecification< DirectMessage > >& spec) {
    std::vector< DirectMessage > result;

    for (auto& item : items) {
        if (spec->isSatisfiedBy(item)) {
            result.push_back(item);
        }
    }

    return result;
}

DMNameSpecification::DMNameSpecification(int user_id, std::string name)
    : m_user_id{user_id}, m_name(name) {}

bool DMNameSpecification::isSatisfiedBy(
    const DirectMessage& direct_message) const {
    int other_user_id = direct_message.get_user_id_1() == m_user_id
                            ? direct_message.get_user_id_2()
                            : direct_message.get_user_id_1();

    std::string name = direct_message.get_user_nickname(other_user_id);

    // check if name is contained in user name
    return name.find(m_name) != std::string::npos;
}

DMLastMessageSpecification::DMLastMessageSpecification(std::string name)
    : m_name{name} {}

bool DMLastMessageSpecification::isSatisfiedBy(
    const DirectMessage& direct_message) const {
    auto msg = ConversationManager::getInstance().get_last_message(
        direct_message.get_conversation_id());

    if (!msg.has_value()) return false;

    return msg.value().get_content().find(m_name) != std::string::npos;
}
