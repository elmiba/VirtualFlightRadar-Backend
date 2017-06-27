/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 3
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 }
 */

#ifndef SRC_TCP_SERVER_SERVER_H_
#define SRC_TCP_SERVER_SERVER_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdint>
#include <boost/thread/mutex.hpp>
#include <string>
#include <vector>

#include "Connection.h"
#include "../../config/Parameters.h"

#define S_MAX_CLIENTS SERVER_MAX_CLIENTS

namespace tcp
{
namespace server
{

/**
 * @class Server
 * @brief A TCP server to serve the same reports to all clients.
 */
class Server
{
public:
    /**
     * Non-copyable
     */
    Server(const Server&) = delete;
    /**
     * Not assignable
     */
    Server& operator=(const Server&) = delete;
    /**
     * @fn Server
     * @brief Constructor
     * @param r_sigset The signal set
     * @param port     The port
     */
    Server(boost::asio::signal_set& r_sigset, std::uint16_t port);
    /**
     * @fn ~Server
     * @brief Destructor
     */
    virtual ~Server() noexcept;
    /**
     * @fn run
     * @brief Run the Server.
     * @note Returns after all operations in the queue have returned.
     */
    void run();
    /**
     * @fn writeToAll
     * @brief Write a message to all clients.
     * @param cr_msg The msg to write
     */
    void writeToAll(const std::string& cr_msg);

private:
    /**
     * @fn accept
     * @brief Accept connections.
     */
    void accept();
    /**
     * @fn awaitStop
     * @brief Register stop-handler to signal set.
     */
    void awaitStop();
    /**
     * @fn stopAll
     * @brief Stop all connections.
     */
    void stopAll();
    /**
     * @fn isConnected
     * @brief Check whether an ip address already exists in the Connection container.
     * @param cr_ip The ip address to check
     * @return true if the ip is already registered, else false
     */
    bool isConnected(const std::string& cr_ip);
    /**
     * @fn handleAccept
     * @brief Handler for accept
     * @param cr_ec The error code
     */
    void handleAccept(const boost::system::error_code& cr_ec) noexcept;

    /// Mutex
    boost::mutex mMutex;
    /// Internal IO-service
    boost::asio::io_service mIOservice;
    /// Ref to signal set
    boost::asio::signal_set& mrSigSet;
    /// Acceptor
    boost::asio::ip::tcp::acceptor mAcceptor;
    /// Socket
    boost::asio::ip::tcp::socket mSocket;
    /// Vector holding Connections
    std::vector<boost::shared_ptr<Connection>> mClients;
};

}  // namespace server
}  // namespace tcp

#endif /* SRC_TCP_SERVER_SERVER_H_ */
