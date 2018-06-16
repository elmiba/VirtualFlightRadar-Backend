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

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "../../Defines.h"
#include "../../Parameters.h"

/// @namespace feed
namespace feed
{
class Feed;

/// @namespace client
namespace client
{
#ifdef CLIENT_CONNECT_WAIT_TIMEVAL
/// @def C_CON_WAIT_TIMEVAL
/// Wait for (re-)connect timeout
#define C_CON_WAIT_TIMEVAL CLIENT_CONNECT_WAIT_TIMEVAL
#else
#define C_CON_WAIT_TIMEVAL 120
#endif

struct Endpoint
{
    const std::string hostName;
    const std::string port;
    std::string ipAddress;

    bool operator==(const Endpoint& crOther) const
    {
        return ipAddress == crOther.ipAddress && port == crOther.port;
    }
};

/**
 * @class Client
 * @brief Base class representing a TCP client.
 * @note A Client is unique and only movable.
 */
class Client
{
public:
    NON_COPYABLE(Client)

    /**
     * @fn ~Client
     * @brief Destructor
     */
    virtual ~Client() noexcept;

    /**
     * @fn run
     * @brief Run the Client.
     * @note Returns after all queued handlers have returned.
     * @param rSigset The signal set reference to register handler
     */
    void run(boost::asio::signal_set& rSigset);

    /**
     * @fn stop
     * @brief Stop the Client and close the connection.
     */
    virtual void stop();

    virtual bool equals(const Client& crOther) const;

    virtual std::size_t hash() const;

protected:
    /**
     * @fn Client
     * @brief Constructor
     * @param crHost       The hostname
     * @param crPort       The port
     * @param crComponent  The component name
     * @param rFeed        The handler Feed reference
     */
    Client(const std::string& crHost, const std::string& crPort,
           const std::string& crComponent);

    /**
     * @fn timedConnect
     * @brief Connect after timeout.
     */
    void timedConnect();

    /**
     * @fn read
     * @brief Read from remote endpoint.
     */
    virtual void read();

    /**
     * @fn connect
     * @brief Connect to host.
     */
    virtual void connect() = 0;

    /**
     * @fn handleTimedConnect
     * @brief Handler for timedConnect.
     * @param crError The error code
     */
    void handleTimedConnect(const boost::system::error_code& crError) noexcept;

    /**
     * @fn handleRead
     * @brief Handler for read.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleRead(const boost::system::error_code& crError,
                    std::size_t vBytes) noexcept;

    /**
     * @fn handleResolve
     * @brief Handler for resolve host.
     * @param crError     The error code
     * @param vResloverIt The resolve iterator
     */
    virtual void
    handleResolve(const boost::system::error_code& crError,
                  boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
        = 0;

    /**
     * @fn handleConnect
     * @brief Handler for connect.
     * @param crError     The error code
     * @param vResolverIt The resolve iterator
     */
    virtual void
    handleConnect(const boost::system::error_code& crError,
                  boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
        = 0;

    /// @var mIoService
    /// Internal IO-service
    boost::asio::io_service mIoService;

    /// @var mSocket
    /// Socket for connections
    boost::asio::ip::tcp::socket mSocket;

    /// @var mResolver
    /// Endpoint resolver
    boost::asio::ip::tcp::resolver mResolver;

    /// @var mResponse
    /// Response string
    std::string mResponse;

    /// @var mBuffer
    /// Read buffer
    boost::asio::streambuf mBuffer;

    /// @var mHost
    /// Hostname
    Endpoint mEndpoint;

    /// @var mComponent
    /// Component string used for logging
    const std::string mComponent;

    /// @var mrFeeds
    /// Handler Feed references
    std::vector<std::shared_ptr<feed::Feed>> mrFeeds;

private:
    /// @var mConnectTimer
    /// Connection timer
    boost::asio::deadline_timer mConnectTimer;

    friend struct ClientHasher;
};

struct ClientHasher
{
    std::size_t operator()(const std::unique_ptr<Client>& crClient) const
    {
        return crClient->hash();
    }
};

struct ClientComparator
{
    bool operator()(const std::unique_ptr<Client>& crClient1,
                    const std::unique_ptr<Client>& crClient2) const
    {
        return crClient1->equals(*crClient2);
    }
};

}  // namespace client
}  // namespace feed
