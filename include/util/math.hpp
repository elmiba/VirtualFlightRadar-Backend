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

#include <algorithm>
#include <cmath>

#include "types.h"

using namespace std::literals;

namespace vfrb::math
{
inline constexpr auto KTS_2_KMH = 1.852;            ///< Convert knots to km/h
inline constexpr auto KMH_2_KTS = 0.539957;         ///< Convert km/h to knots
inline constexpr auto KTS_2_MS  = 0.514444;         ///< Convert knots to m/s
inline constexpr auto MS_2_KTS  = 1.94384;          ///< Convert m/s to knots
inline constexpr auto KMH_2_MS  = 0.277778;         ///< Convert km/h to m/s
inline constexpr auto MS_2_KMH  = 3.6;              ///< Convert m/s to km/h
inline constexpr auto MS_2_FPM  = 196.85;           ///< Convert m/s to fpm
inline constexpr auto FPM_2_MS  = 0.00508;          ///< Convert fpm to m/s
inline constexpr auto FEET_2_M  = 0.3048;           ///< Convert feet to m
inline constexpr auto M_2_FEET  = 3.28084;          ///< Convert m to feet
const auto            PI        = std::acos(-1.0);  ///< The circular number

/**
 * @brief Convert degree to radian.
 * @param degree The degrees
 * @return the radian
 */
inline f64 Radian(f64 degree_)
{
    return ((degree_ * PI) / 180.0);
}

/**
 * @brief Convert radian to degree.
 * @param radian The radian
 * @return the degrees
 */
inline f64 Degree(f64 radian_)
{
    return (radian_ * 180.0) / PI;
}

/**
 * @brief Convert f64 to int, round to nearest number.
 * @param value The floating point value
 * @return the rounded integer
 */
inline s32 DoubleToInt(f64 value_)
{
    return (value_ >= 0.0) ? static_cast<s32>(value_ + 0.5) : static_cast<s32>(value_ - 0.5);
}

/**
 * @brief Convert ( degree, minute-as-decimal ) to degree.
 * @param degMin The degree-minute value
 * @return the degree value
 */
inline f64 DmToDeg(f64 degMin_)
{
    f64 absDm = std::abs(degMin_ / 100.0);
    f64 d     = std::floor(absDm);
    f64 m     = (absDm - d) * 100.0 / 60.0;
    return d + m;
}

template<typename T>
T Saturate(T val_, T min_, T max_)
{
    return std::max(min_, std::min(val_, max_));
}

/**
 * @brief Calculate height difference from QNE to Pressure in meters with ICAO height
 * formula.
 * @param pressure The air pressure
 * @return the height difference to QNE
 */
inline s32 IcaoHeight(f64 pressure_)
{
    return DoubleToInt(288.15 * (1.0 - std::pow((pressure_ / 1013.25), 0.190295)) / 0.0065);
}
}  // namespace vfrb::math
