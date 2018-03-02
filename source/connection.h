//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////

#ifndef __OTSERV_CONNECTION_H__
#define __OTSERV_CONNECTION_H__

#include "definitions.h"
#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "networkmessage.h"

class Protocol;
class OutputMessage;
typedef boost::shared_ptr<OutputMessage> OutputMessage_ptr;
class Connection;
typedef boost::shared_ptr<Connection> Connection_ptr;
class ServiceBase;
typedef boost::shared_ptr<ServiceBase> Service_ptr;
class ServicePort;
typedef boost::shared_ptr<ServicePort> ServicePort_ptr;

#ifdef __DEBUG_NET__
#define PRINT_ASIO_ERROR(desc) \
	std::cout << "Error: [" << __FUNCTION__ << "] " << desc << " - Error: " <<  \
		error.value() << " Desc: " << error.message() << std::endl;
#else
#define PRINT_ASIO_ERROR(desc)
#endif

class ConnectionManager
{
public:
	~ConnectionManager()
	{
	}

	static ConnectionManager* getInstance(){
		static ConnectionManager instance;
		return &instance;
	}

	Connection_ptr createConnection(boost::asio::ip::tcp::socket* socket,
		boost::asio::io_service& io_service, ServicePort_ptr servicers);
	void releaseConnection(Connection_ptr connection);
	void closeAll();

protected:

	ConnectionManager()
	{
	}

	std::list<Connection_ptr> m_connections;
	boost::recursive_mutex m_connectionManagerLock;
};

class Connection : public boost::enable_shared_from_this<Connection>, boost::noncopyable
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t connectionCount;
#endif

	enum { write_timeout = 30 };
	enum { read_timeout = 30 };

	enum ConnectionState_t {
		CONNECTION_STATE_OPEN = 0,
		CONNECTION_STATE_REQUEST_CLOSE = 1,
		CONNECTION_STATE_CLOSING = 2,
		CONNECTION_STATE_CLOSED = 3
	};

private:
	Connection(boost::asio::ip::tcp::socket* socket,
		boost::asio::io_service& io_service,
		ServicePort_ptr service_port) :
			m_socket(socket),
			m_readTimer(io_service),
			m_writeTimer(io_service),
			m_io_service(io_service),
			m_service_port(service_port)
	{
		m_refCount = 0;
		m_protocol = NULL;
		m_pendingWrite = 0;
		m_pendingRead = 0;
		m_connectionState = CONNECTION_STATE_OPEN;
		m_receivedFirst = false;
		m_writeError = false;
		m_readError = false;
		m_packetsSent = 0;
		m_timeConnected = time(NULL);

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
		connectionCount++;
#endif
	}
	friend class ConnectionManager;

public:
	~Connection()
	{
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
		connectionCount--;
#endif
	}

	boost::asio::ip::tcp::socket& getHandle() { return *m_socket; }

	void closeConnection();
	// Used by protocols that require server to send first
	void acceptConnection(Protocol* protocol);
	void acceptConnection();

	bool send(OutputMessage_ptr msg);

	uint32_t getIP() const;

	int32_t addRef() {return ++m_refCount;}
	int32_t unRef() {return --m_refCount;}

private:
	void parseHeader(const boost::system::error_code& error);
	void parsePacket(const boost::system::error_code& error);

	void onWriteOperation(OutputMessage_ptr msg, const boost::system::error_code& error);

	void onStopOperation();
	void handleReadError(const boost::system::error_code& error);
	void handleWriteError(const boost::system::error_code& error);

	static void handleReadTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error);
	static void handleWriteTimeout(boost::weak_ptr<Connection> weak_conn, const boost::system::error_code& error);

	void closeConnectionTask();
	void deleteConnectionTask();
	void releaseConnection();
	void closeSocket();
	void onReadTimeout();
	void onWriteTimeout();

	void internalSend(OutputMessage_ptr msg);

	NetworkMessage m_msg;
	boost::asio::ip::tcp::socket* m_socket;
	boost::asio::deadline_timer m_readTimer;
	boost::asio::deadline_timer m_writeTimer;
	boost::asio::io_service& m_io_service;
	ServicePort_ptr m_service_port;
	bool m_receivedFirst;
	bool m_writeError;
	bool m_readError;
	time_t m_timeConnected;
	uint32_t m_packetsSent;

	int32_t m_pendingWrite;
	int32_t m_pendingRead;
	ConnectionState_t m_connectionState;
	uint32_t m_refCount;
	static bool m_logError;
	boost::recursive_mutex m_connectionLock;

	Protocol* m_protocol;
};

#endif
