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

#include "util/Logger.hpp"

#include "helper.hpp"

using namespace sctf;

TEST_FUNCTION(test_config)
TEST_FUNCTION(test_data)
TEST_FUNCTION(test_data_processor)
TEST_FUNCTION(test_feed_parser)
TEST_FUNCTION(test_object)
TEST_FUNCTION(test_math)
TEST_FUNCTION(test_client)
TEST_FUNCTION(test_server)
TEST_FUNCTION(test_feed)

int main(int, char**)
{
    logger.set_logFile("/dev/null");
    // auto rep = createXmlReporter();
    auto                   rep = createPlainTextReporter(true);
    test::TestSuitesRunner runner;

    test_config(runner);
    test_data(runner);
    test_data_processor(runner);
    test_feed_parser(runner);
    test_object(runner);
    test_math(runner);
    test_feed(runner);
    test_client(runner);
    test_server(runner);

    return rep->report(runner) > 0 ? 1 : 0;
}
