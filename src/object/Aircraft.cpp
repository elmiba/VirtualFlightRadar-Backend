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

#include "object/Aircraft.h"

#include <typeinfo>

namespace object
{
Aircraft::Aircraft() : Aircraft(0) {}

Aircraft::Aircraft(std::uint32_t priority)
    : Object(priority),
      m_idType(IdType::ICAO),
      m_aircraftType(AircraftType::POWERED_AIRCRAFT),
      m_targetType(TargetType::TRANSPONDER)
{}

void Aircraft::assign(Object&& other)
{
    try
    {
        Aircraft&& update = dynamic_cast<Aircraft&&>(other);
        Object::assign(std::move(other));
        this->m_idType       = update.m_idType;
        this->m_aircraftType = update.m_aircraftType;
        this->m_targetType   = update.m_targetType;
        this->m_position     = update.m_position;
        this->m_movement     = update.m_movement;
        this->m_timeStamp    = update.m_timeStamp;
        this->m_fullInfo     = update.m_fullInfo;
    }
    catch (const std::bad_cast&)
    {}
}

bool Aircraft::canUpdate(const Object& other) const
{
    try
    {
        const Aircraft& toUpdate = dynamic_cast<const Aircraft&>(other);
        return (this->m_timeStamp > toUpdate.m_timeStamp) &&
               (toUpdate.m_targetType == TargetType::TRANSPONDER ||
                this->m_targetType == TargetType::FLARM) &&
               Object::canUpdate(other);
    }
    catch (const std::bad_cast&)
    {
        return false;
    }
}

void Aircraft::set_aircraftType(Aircraft::AircraftType type)
{
    m_aircraftType = type > AircraftType::STATIC_OBJECT ? AircraftType::UNKNOWN : type;
}

void Aircraft::set_idType(Aircraft::IdType type)
{
    m_idType = type > IdType::OGN ? IdType::RANDOM : type;
}

}  // namespace object
