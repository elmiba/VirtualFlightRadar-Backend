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

#include "ParserSBS.h"
#include "Configuration.h"
#include "Math.h"

ParserSBS::ParserSBS()
: Parser()
{
}

ParserSBS::~ParserSBS()
{
}

int ParserSBS::unpack(const std::string& sentence, AircraftContainer& ac_cont)
{
    /*
     * fields:
     * 4 : id
     * 7 : time
     * 11: altitude
     * 14: latitude
     * 15: longitude
     */
    int delim, i = 2, p = 6;
    while((delim = sentence.find(',',p)) != std::string::npos && i < 16) {
        switch(i) {
            case 4:
                if (delim - p > 0) {
                    id = sentence.substr(p,delim-p);
                } else return -1;
                break;
            case 7:
                try {
                    if (delim - p > 7) {
                        time = std::stoi(sentence.substr(p,2)) * 10000;
                        time += std::stoi(sentence.substr(p+3,2)) * 100;
                        time += std::stoi(sentence.substr(p+6,2));
                    } else return -1;
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            case 11:
                try {
                    alt = Math::dToI(std::stod(sentence.substr(p,delim-p)) * Math::feet2m);
                    if (alt > Configuration::filter_maxHeight) return -1;
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            case 14:
                try {
                    lat = std::stod(sentence.substr(p,delim-p));
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            case 15:
                try {
                    lon = std::stod(sentence.substr(p,delim-p));
                } catch (std::logic_error& e) {
                    return -1;
                }
                break;
            default:
                break;
        }
        i++;
        p = delim+1;
    }
    ac_cont.insertAircraft(id, lat, lon, alt);
    return 0;
}
