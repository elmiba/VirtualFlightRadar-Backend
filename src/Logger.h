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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <mutex>

class Logger
{
public:
    Logger();
    virtual ~Logger();

    static void info(const char*, const char*);
    static void warn(const char*, const char*);
    static void error(const char*, const char*);

private:

    static std::mutex mutex;
};

#endif /* SRC_LOGGER_H_ */
