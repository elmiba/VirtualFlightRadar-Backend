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

#include "client/GpsdClient.h"

#include <chrono>
#include <condition_variable>
#include <thread>
#include <utility>

#include "util/Logger.hpp"

#ifdef COMPONENT
#    undef COMPONENT
#endif
#define COMPONENT "(GpsdClient)"

namespace client
{
using namespace net;

GpsdClient::GpsdClient(const Endpoint& endpoint, std::shared_ptr<Connector> connector)
    : Client(endpoint, COMPONENT, connector)
{}

void GpsdClient::handleConnect(ErrorCode error)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    if (m_state == State::CONNECTING)
    {
        if (error == ErrorCode::SUCCESS)
        {
            m_connector->onWrite("?WATCH={\"enable\":true,\"nmea\":true}\r\n",
                                 std::bind(&GpsdClient::handleWatch, this, std::placeholders::_1));
        }
        else
        {
            logger.warn(m_component, " failed to connect to ", m_endpoint.host, ":",
                        m_endpoint.port);
            reconnect();
        }
    }
}

void GpsdClient::stop()
{
    if (m_state == State::RUNNING)
    {
        std::mutex                   sync;
        std::unique_lock<std::mutex> lk(sync);
        std::condition_variable      cv;
        bool                         sent = false;
        m_connector->onWrite("?WATCH={\"enable\":false}\r\n", [&](ErrorCode) {
            logger.info(m_component, " stopped watch");
            sent = true;
            cv.notify_one();
        });
        cv.wait_for(lk, std::chrono::milliseconds(500), [&] { return sent; });
    }
    Client::stop();
}

void GpsdClient::handleWatch(ErrorCode error)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    if (m_state == State::CONNECTING)
    {
        if (error == ErrorCode::SUCCESS)
        {
            m_state = State::RUNNING;
            logger.info(m_component, " connected to ", m_endpoint.host, ":", m_endpoint.port);
            read();
        }
        else
        {
            logger.error(m_component, " send watch request failed");
            reconnect();
        }
    }
}
}  // namespace client
