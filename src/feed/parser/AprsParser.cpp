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

#include "feed/parser/AprsParser.h"

#include <limits>
#include <stdexcept>

#include "object/GpsPosition.h"
#include "object/TimeStamp.hpp"
#include "object/impl/DateTimeImplBoost.h"
#include "util/math.hpp"

/// @def RE_APRS_TIME
/// APRS regex match group of time
#define RE_APRS_TIME 1

/// @def RE_APRS_LAT
/// APRS regex match group of latitude
#define RE_APRS_LAT 2

/// @def RE_APRS_LAT_DIR
/// APRS regex match group of latitude orientation
#define RE_APRS_LAT_DIR 3

/// @def RE_APRS_LON
/// APRS regex match group of longitude
#define RE_APRS_LON 4

/// @def RE_APRS_LON_DIR
/// APRS regex match group of longitude orientation
#define RE_APRS_LON_DIR 5

/// @def RE_APRS_HEAD
/// APRS regex match group of heading
#define RE_APRS_HEAD 6

/// @def RE_APRS_GND_SPD
/// APRS regex match group of ground speed
#define RE_APRS_GND_SPD 7

/// @def RE_APRS_ALT
/// APRS regex match group of altitude
#define RE_APRS_ALT 8

/// @def RE_APRS_COM
/// APRS regex match group of comment
#define RE_APRS_COM 9

/// @def RE_APRS_COM_TYPE
/// APRS regex match group of id and aircraft type
#define RE_APRS_COM_TYPE 1

/// @def RE_APRS_COM_ID
/// APRS regex match group of aircraft id
#define RE_APRS_COM_ID 2

/// @def RE_APRS_COM_CR
/// APRS regex match group of climb rate
#define RE_APRS_COM_CR 3

/// @def RE_APRS_COM_TR
/// APRS regex match group of turn rate
#define RE_APRS_COM_TR 4

namespace feed
{
using namespace object;

namespace parser
{
const boost::regex AprsParser::s_APRS_RE(
    "^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
    boost::regex::optimize | boost::regex::icase);

const boost::regex AprsParser::s_APRSExtRE(
    "^(?:[\\S\\s]+?)?id([0-9A-F]{2})([0-9A-F]{6})\\s?(?:([\\+-]\\d{3})fpm\\s+?)?(?:([\\+-]\\d+?\\.\\d+?)rot)?(?:[\\S\\s]+?)?$",
    boost::regex::optimize | boost::regex::icase);

std::int32_t AprsParser::s_maxHeight = std::numeric_limits<std::int32_t>::max();

AprsParser::AprsParser() : Parser<Aircraft>() {}

bool AprsParser::unpack(const std::string& sentence, Aircraft& aircraft) noexcept
{
    boost::smatch match, com_match;

    if ((!sentence.empty() && sentence.front() == '#') ||
        !(boost::regex_match(sentence, match, s_APRS_RE) && parsePosition(match, aircraft) &&
          parseTimeStamp(match, aircraft)))
    {
        return false;
    }
    std::string comm(match.str(RE_APRS_COM));

    if (!(boost::regex_match(comm, com_match, s_APRSExtRE) && parseComment(com_match, aircraft)))
    {
        return false;
    }
    aircraft.set_fullInfo(parseMovement(match, com_match, aircraft));
    aircraft.set_targetType(Aircraft::TargetType::FLARM);
    return true;
}

bool AprsParser::parsePosition(const boost::smatch& match, Aircraft& aircraft) noexcept
{
    bool valid = false;
    try
    {
        Position pos;
        pos.latitude = math::dmToDeg(std::stod(match.str(RE_APRS_LAT)));
        if (match.str(RE_APRS_LAT_DIR).compare("S") == 0)
        {
            pos.latitude = -pos.latitude;
        }
        pos.longitude = math::dmToDeg(std::stod(match.str(RE_APRS_LON)));
        if (match.str(RE_APRS_LON_DIR).compare("W") == 0)
        {
            pos.longitude = -pos.longitude;
        }
        pos.altitude = math::doubleToInt(std::stod(match.str(RE_APRS_ALT)) * math::FEET_2_M);

        aircraft.set_position(pos);
        valid = pos.altitude <= s_maxHeight;
    }
    catch (const std::logic_error&)
    {}
    return valid;
}

bool AprsParser::parseComment(const boost::smatch& match, Aircraft& aircraft) noexcept
{
    aircraft.set_id(match.str(RE_APRS_COM_ID));
    try
    {
        aircraft.set_idType(static_cast<Aircraft::IdType>(
            std::stoi(match.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x03));
        aircraft.set_aircraftType(static_cast<Aircraft::AircraftType>(
            (std::stoi(match.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x7C) >> 2));
    }
    catch (const std::logic_error&)
    {
        return false;
    }
    return true;
}

bool AprsParser::parseMovement(const boost::smatch& match, const boost::smatch& comMatch,
                               Aircraft& aircraft) noexcept
{
    Movement move;
    // This needs to be split later to parse independently.
    try
    {
        move.heading   = std::stod(match.str(RE_APRS_HEAD));
        move.gndSpeed  = std::stod(match.str(RE_APRS_GND_SPD)) * math::KTS_2_MS;
        move.climbRate = std::stod(comMatch.str(RE_APRS_COM_CR)) * math::FPM_2_MS;
    }
    catch (const std::logic_error&)
    {
        return false;
    }
    aircraft.set_movement(move);
    return true;
}

bool AprsParser::parseTimeStamp(const boost::smatch& match, Aircraft& aircraft) noexcept
{
    try
    {
        aircraft.set_timeStamp(TimeStamp<timestamp::DateTimeImplBoost>(match.str(RE_APRS_TIME),
                                                                       timestamp::Format::HHMMSS));
    }
    catch (const std::invalid_argument&)
    {
        return false;
    }
    return true;
}

}  // namespace parser
}  // namespace feed
