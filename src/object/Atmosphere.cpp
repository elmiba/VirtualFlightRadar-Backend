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

#include "object/Atmosphere.h"

#include <typeinfo>

namespace object
{
Atmosphere::Atmosphere() : Object() {}

Atmosphere::Atmosphere(std::uint32_t priority) : Object(priority) {}

Atmosphere::Atmosphere(double pressure, std::uint32_t priority)
    : Object(priority), m_pressure(pressure)
{}

void Atmosphere::assign(Object&& other)
{
    try
    {
        Atmosphere&& update = dynamic_cast<Atmosphere&&>(other);
        Object::assign(std::move(other));
        this->m_pressure = update.m_pressure;
    }
    catch (const std::bad_cast&)
    {}
}

}  // namespace object
