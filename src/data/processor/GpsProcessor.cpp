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

#include "data/processor/GpsProcessor.h"

#include <cmath>
#include <cstdio>

#include "util/Logger.hpp"
#include "util/math.hpp"

static auto const& logger = Logger::instance();

using namespace object;

namespace data::processor
{
GpsProcessor::GpsProcessor() : Processor<object::GpsPosition>() {}

void GpsProcessor::process(object::GpsPosition& position) const
{
    std::time_t now = std::time(nullptr);
    std::tm*    utc = std::gmtime(&now);
    evalPosition(position.location().latitude, position.location().longitude);
    appendGPGGA(position, utc, appendGPRMC(position, utc, 0));
}

usize GpsProcessor::appendGPGGA(GpsPosition& position, std::tm const* utc, usize pos) const
{
    // As we use XCSoar as frontend, we need to set the fix quality to 1. It doesn't
    // support others.
    // "$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,%1d,%02d,1,%d,M,%.1lf,M,,*"
    int next = (*position).format(
        pos, "$GPGGA,%.2d%.2d%.2d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,1,%.2hhu,1,%d,M,%.1lf,M,,*",
        utc->tm_hour, utc->tm_min, utc->tm_sec, m_degLatitude, m_minLatitude, m_directionSN, m_degLongitude,
        m_minLongitude, m_directionEW, /*pos.fixQa,*/ position.nrOfSatellites(), position.location().altitude,
        math::saturate(position.geoid(), GpsPosition::MIN_GEOID, GpsPosition::MAX_GEOID));
    logger.info("written ", next, " of ", GpsPosition::NMEA_SIZE, " at ", pos);
    next += (*position).format(pos, "%02x\r\n", math::checksum(**position, pos));
    logger.info("written ", next, " of ", GpsPosition::NMEA_SIZE, " at ", pos);
    return pos + static_cast<usize>(next);
}

usize GpsProcessor::appendGPRMC(GpsPosition& position, std::tm const* utc, usize pos) const
{
    int next = (*position).format(
        pos, "$GPRMC,%.2d%.2d%.2d,A,%02.0lf%06.3lf,%c,%03.0lf%06.3lf,%c,0,0,%.2d%.2d%.2d,001.0,W*",
        utc->tm_hour, utc->tm_min, utc->tm_sec, m_degLatitude, m_minLatitude, m_directionSN, m_degLongitude,
        m_minLongitude, m_directionEW, utc->tm_mday, utc->tm_mon + 1, utc->tm_year - 100);
    logger.info("written ", next, " of ", GpsPosition::NMEA_SIZE, " at ", pos);
    next += (*position).format(pos, "%02x\r\n", math::checksum(**position, pos));
    logger.info("written ", next, " of ", GpsPosition::NMEA_SIZE, " at ", pos);
    return pos + static_cast<usize>(next);
}

void GpsProcessor::evalPosition(f64 latitude, f64 longitude) const
{
    m_directionSN  = (latitude < 0) ? 'S' : 'N';
    m_directionEW  = (longitude < 0) ? 'W' : 'E';
    m_degLatitude  = std::abs(std::floor(latitude));
    m_minLatitude  = std::abs(60.0 * (latitude - m_degLatitude));
    m_degLongitude = std::abs(std::floor(longitude));
    m_minLongitude = std::abs(60.0 * (longitude - m_degLongitude));
}
}  // namespace data::processor
