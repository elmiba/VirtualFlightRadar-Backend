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

#include <cstdint>
#include <functional>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "SocketImplBoost.h"

#include "../Defines.h"

namespace server
{
class TcpInterfaceImplBoost
{
public:
    NOT_COPYABLE(TcpInterfaceImplBoost)

    explicit TcpInterfaceImplBoost(std::uint16_t vPort);
    ~TcpInterfaceImplBoost() noexcept;

    void run();
    void stop();
    void onAccept(const std::function<void(bool) noexcept>& crCallback);
    void close();
    SocketImplBoost& getSocket();

private:
    void handleAccept(const boost::system::error_code& crError,
                      const std::function<void(bool) noexcept>& crCallback) noexcept;

    boost::asio::io_service mIoService;
    boost::asio::ip::tcp::acceptor mAcceptor;
    SocketImplBoost mSocket;
};

}  // namespace server
