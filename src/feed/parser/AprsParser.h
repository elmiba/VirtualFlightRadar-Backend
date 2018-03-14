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

#include <cstdint>
#include <string>
#include <boost/regex.hpp>

#include "../../data/object/Aircraft.h"
#include "Parser.hpp"

namespace feed
{
namespace parser
{
/**
 * @class AprsParser implements Parser
 * @brief Provide unpacking method for APRS sentences.
 * @see Parser.hpp
 * @see ../aircraft/Aircraft.hpp
 */
class AprsParser : public Parser<data::object::Aircraft>
{
public:
    AprsParser();

    /**
     * @fn AprsParser
     * @brief Constructor
     */
    explicit AprsParser(std::int32_t vMaxHeight);

    /**
     * @fn ~AprsParser
     * @brief Destructor
     */
    virtual ~AprsParser() noexcept;

    /**
     * @fn unpack
     * @brief Unpack into Aircraft.
     * @override Parser::unpack
     */
    bool unpack(const std::string& crStr,
                data::object::Aircraft& rAircraft) noexcept override;

private:
    // cppcheck-suppress unusedPrivateFunction
    bool parsePosition(const boost::smatch& crMatch, data::object::Aircraft& rAircraft);

    // cppcheck-suppress unusedPrivateFunction
    bool parseComment(const boost::smatch& crMatch, data::object::Aircraft& rAircraft);

    // cppcheck-suppress unusedPrivateFunction
    bool parseMovement(const boost::smatch& crMatch, data::object::Aircraft& rAircraft);

    /// Regular expression for APRS protocol
    static const boost::regex msAprsRe;
    /// Regular expression for OGN specific APRS extension
    static const boost::regex msAprsComRe;

    const std::int32_t mMaxHeight;
};
}
}  // namespace parser
