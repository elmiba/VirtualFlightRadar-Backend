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

#include "feed/WindFeed.h"

#include "config/Configuration.h"
#include "data/WindData.h"
#include "feed/parser/WindParser.h"
#include "object/Wind.h"

using namespace vfrb::config;

namespace vfrb::feed
{
WindFeed::WindFeed(Str const& name, Properties const& properties, SPtr<data::WindData> data)
    : Feed(name, properties, data)
{}

Feed::Protocol WindFeed::protocol() const
{
    return Protocol::SENSOR;
}

bool WindFeed::process(Str response)
{
    try
    {
        m_data->update(m_parser.unpack(std::move(response), m_priority));
    }
    catch ([[maybe_unused]] parser::error::UnpackError const&)
    {}
    return true;
}
}  // namespace vfrb::feed
