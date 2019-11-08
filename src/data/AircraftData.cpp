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

#include "data/AircraftData.h"

#include "error/Error.hpp"

using namespace vfrb::object;

namespace vfrb::data
{
AircraftData::AircraftData(AccessFn&& fn) : AircraftData(std::move(fn), 0) {}

AircraftData::AircraftData(AccessFn&& fn, s32 maxDist) : Data(std::move(fn)), m_processor(maxDist) {}

bool AircraftData::update(CObject&& aircraft)
{
    auto&& update = static_cast<CAircraft&&>(aircraft);
    auto   result = m_container.insert(std::hash<std::string_view>()(*update.Id()), std::move(update));
    if (!result.second)
    {
        result.first->value.tryUpdate(std::move(update));
    }
    return true;
}

void AircraftData::environment(SLocation const& position, f64 atmPress)
{
    m_processor.referTo(position, atmPress);
}

void AircraftData::access()
{
    auto iter = m_container.begin();
    while (iter != m_container.end())
    {
        ++(iter->value);
        try
        {
            if (iter->value.updateAge() == NO_FLARM_THRESHOLD)
            {
                iter->value.targetType(CAircraft::ETargetType::TRANSPONDER);
            }
            if (iter->value.updateAge() >= DELETE_THRESHOLD)
            {
                auto key = iter.key();
                ++iter;
                m_container.erase(key);
            }
            else
            {
                m_processor.process(iter->value, iter->nmea);
                m_accessFn({iter->value, iter->nmea});
                ++iter;
            }
        }
        catch ([[maybe_unused]] vfrb::error::IError const&)
        {}
    }
}
}  // namespace vfrb::data
