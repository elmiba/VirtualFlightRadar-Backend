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

#include "data/processor/AircraftProcessor.h"

#include <cmath>
#include <limits>

#include "util/math.hpp"
#include "util/utility.hpp"

using namespace object;

namespace data
{
namespace processor
{
AircraftProcessor::AircraftProcessor() : AircraftProcessor(std::numeric_limits<std::int32_t>::max()) {}

AircraftProcessor::AircraftProcessor(std::int32_t maxDist)
    : Processor<object::Aircraft>(), m_maxDistance(maxDist)
{}

void AircraftProcessor::process(Aircraft& aircraft) const
{
    calculateRelPosition(aircraft);
    if (m_distance <= m_maxDistance)
    {
        appendPFLAA(aircraft, appendPFLAU(aircraft, 0));
    }
    else
    {
        (*aircraft).clear();
    }
}

void AircraftProcessor::referTo(const Location& location, double atmPress)
{
    m_refLocation    = location;
    m_refAtmPressure = atmPress;
}

void AircraftProcessor::calculateRelPosition(const Aircraft& aircraft) const
{
    m_refRadLatitude       = math::radian(m_refLocation.latitude);
    m_refRadLongitude      = math::radian(m_refLocation.longitude);
    m_aircraftRadLongitude = math::radian(aircraft.getLocation().longitude);
    m_aircraftRadLatitude  = math::radian(aircraft.getLocation().latitude);
    m_lonDistance          = m_aircraftRadLongitude - m_refRadLongitude;
    m_latDistance          = m_aircraftRadLatitude - m_refRadLatitude;

    double a = std::pow(std::sin(m_latDistance / 2.0), 2.0) +
               std::cos(m_refRadLatitude) * std::cos(m_aircraftRadLatitude) *
                   std::pow(std::sin(m_lonDistance / 2.0), 2.0);
    m_distance = math::doubleToInt(6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    m_relBearing = math::degree(
        std::atan2(std::sin(m_aircraftRadLongitude - m_refRadLongitude) * std::cos(m_aircraftRadLatitude),
                   std::cos(m_refRadLatitude) * std::sin(m_aircraftRadLatitude) -
                       std::sin(m_refRadLatitude) * std::cos(m_aircraftRadLatitude) *
                           std::cos(m_aircraftRadLongitude - m_refRadLongitude)));
    m_absBearing = std::fmod((m_relBearing + 360.0), 360.0);

    m_relNorth    = math::doubleToInt(std::cos(math::radian(m_absBearing)) * m_distance);
    m_relEast     = math::doubleToInt(std::sin(math::radian(m_absBearing)) * m_distance);
    m_relVertical = aircraft.getTargetType() == Aircraft::TargetType::TRANSPONDER ?
                        aircraft.getLocation().altitude - math::icaoHeight(m_refAtmPressure) :
                        aircraft.getLocation().altitude - m_refLocation.altitude;
}

std::size_t AircraftProcessor::appendPFLAU(Aircraft& aircraft, std::size_t pos) const
{
    int bytes =
        (*aircraft).snprintf(pos, Aircraft::NMEA_SIZE - pos, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
                             math::doubleToInt(m_relBearing), m_relVertical, m_distance, *aircraft.getId());
    bytes += (*aircraft).snprintf(pos, Aircraft::NMEA_SIZE - pos - static_cast<std::size_t>(bytes),
                                  "%02x\r\n", math::checksum(**aircraft, Aircraft::NMEA_SIZE - pos));
    return pos + static_cast<std::size_t>(bytes);
}

std::size_t AircraftProcessor::appendPFLAA(Aircraft& aircraft, std::size_t pos) const
{
    int bytes = 0;
    if (aircraft.hasFullInfo())
    {
        bytes = (*aircraft).snprintf(
            pos, Aircraft::NMEA_SIZE - pos, "$PFLAA,0,%d,%d,%d,%hhu,%s,%.3d,,%d,%3.1lf,%.1hhX*", m_relNorth,
            m_relEast, m_relVertical, util::raw_type(aircraft.getIdType()), *aircraft.getId(),
            math::doubleToInt(math::saturate(aircraft.getMovement().heading, Aircraft::Movement::MIN_HEADING,
                                             Aircraft::Movement::MAX_HEADING)),
            math::doubleToInt(math::saturate(aircraft.getMovement().gndSpeed * math::MS_2_KMH,
                                             Aircraft::Movement::MIN_GND_SPEED,
                                             Aircraft::Movement::MAX_GND_SPEED)),
            math::saturate(aircraft.getMovement().climbRate, Aircraft::Movement::MIN_CLIMB_RATE,
                           Aircraft::Movement::MAX_CLIMB_RATE),
            util::raw_type(aircraft.getAircraftType()));
    }
    else
    {
        bytes = (*aircraft).snprintf(pos, Aircraft::NMEA_SIZE - pos, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1hhX*",
                                     m_relNorth, m_relEast, m_relVertical, *aircraft.getId(),
                                     util::raw_type(aircraft.getAircraftType()));
    }
    bytes += (*aircraft).snprintf(pos, Aircraft::NMEA_SIZE - pos - static_cast<std::size_t>(bytes),
                                  "%02x\r\n", math::checksum(**aircraft, Aircraft::NMEA_SIZE - pos));
    return pos + static_cast<std::size_t>(bytes);
}

}  // namespace processor
}  // namespace data
