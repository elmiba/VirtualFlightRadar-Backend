/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#ifndef OUTPUTCONNECTION_H_
#define OUTPUTCONNECTION_H_

#include <netinet/in.h>
#include <sys/un.h>
#include <cstdint>

#include "Connection.h"

class OutputConnection: public Connection
{
public:
    OutputConnection(sa_family_t family, in_port_t port);
    virtual ~OutputConnection() throw ();

    void fillAddr();
    void listenToSocket(uint32_t);
};

#endif /* OUTPUTCONNECTION_H_ */
