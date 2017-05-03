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

#include "SensorParser.h"

#include <stdexcept>

#include "../data/SensorData.h"
#include "../util/Math.hpp"
#include "../vfrb/VFRB.h"

SensorParser::SensorParser()
        : Parser()
{
}

SensorParser::~SensorParser() noexcept
{
}

std::int32_t SensorParser::unpack(const std::string& r_msg, Priority prio)
        noexcept
        {
    try
    {
        std::int32_t csum = std::stoi(r_msg.substr(r_msg.rfind('*') + 1, 2),
                nullptr, 16);
        if (csum != Math::checksum(r_msg.c_str(), r_msg.length()))
        {
            return MSG_UNPACK_IGN;
        }
    } catch (const std::logic_error& e)
    {
        return MSG_UNPACK_ERR;
    }

    if (r_msg.find("MDA") != std::string::npos)
    {
        try
        {
            mtB = r_msg.find('B') - 1;
            mtS = r_msg.substr(0, mtB).find_last_of(',') + 1;
            mtSubLen = mtB - mtS;
            mtPress = std::stod(r_msg.substr(mtS, mtSubLen), &mtNumIdx) * 1000.0;
            if (mtNumIdx == mtSubLen)
            {
                VFRB::msSensorData.setPress(prio, mtPress);
            } else
            {
                return MSG_UNPACK_ERR;
            }
        } catch (std::logic_error& e)
        {
            return MSG_UNPACK_ERR;
        }
    } else if (r_msg.find("MWV") != std::string::npos)
    {
        VFRB::msSensorData.setWVstr(prio, r_msg);
    } else
    {
        return MSG_UNPACK_IGN;
    }
    return MSG_UNPACK_SUC;
}