#include "Session.hpp"

Session::Session() {}

Session::Session(int session_id, int user_id)
    : m_session_id(session_id), m_user_id(user_id) {
    // expiration_date = date::today() + days(????);
}

void Session::set_session_id(int session_id) { m_session_id = session_id; }

int Session::get_session_id() const { return m_session_id; }

void Session::set_user_id(int user_id) { m_user_id = user_id; }

int Session::get_user_id() const { return m_user_id; }

std::ostream& operator<<(std::ostream& out, const Session& session) {
	out << "Session ID: " << session.m_session_id << "\nUser ID: "
		<< session.m_user_id << "\n";
	return out;
}

void Session::Serialize(Walnut::StreamWriter* serializer,
                        const Session& instance) {
    serializer->WriteRaw< int >(instance.m_session_id);
    serializer->WriteRaw< int >(instance.m_user_id);
}

void Session::Deserialize(Walnut::StreamReader* deserializer,
                          Session& instance) {
    deserializer->ReadRaw< int >(instance.m_session_id);
    deserializer->ReadRaw< int >(instance.m_user_id);
}