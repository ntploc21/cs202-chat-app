#include "Request.hpp"

void Request::set_client_id(Walnut::ClientID client_id) {
    this->m_client_id = client_id;
}

Walnut::ClientID Request::get_client_id() const {
	return this->m_client_id; }

void Request::set_session_id(int session) { this->m_session_id = session; }

int Request::get_session_id() const { return this->m_session_id; }

