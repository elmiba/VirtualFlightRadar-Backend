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

#include <string>
#include <sstream>

#include "../../src/config/ConfigReader.h"
#include "../../src/config/Configuration.h"
#include "../framework/src/comparator/Comparators.hpp"
#include "../framework/src/comparator/ComparatorStrategy.hpp"
#include "../framework/src/testsuite/TestSuite.hpp"
#include "../framework/src/testsuite/TestSuitesRunner.hpp"
#include "../framework/src/util/assert.hpp"
#include "../Helper.hpp"

#ifdef assert
#undef assert
#endif

using namespace util;
using namespace testsuite;
using namespace comparator;

void test_config(TestSuitesRunner& runner) {
	describe<config::ConfigReader>("read config", runner)->test("read",
			[]()
			{
				std::stringstream conf;
				conf << "[" << SECT_KEY_FALLBACK << "]\n" << KV_KEY_LATITUDE << "   = 0.000000\n";
				conf << KV_KEY_LONGITUDE << " = \n" << KV_KEY_ALTITUDE << "=1000; alt\n;ghsgd";
				config::ConfigReader cr;
				cr.read(conf);
				assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "invalid"), std::string("0.000000"), helper::eqs);
				assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "invalid"), std::string("invalid"), helper::eqs);
				assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "invalid"), std::string("1000"), helper::eqs);
				assert(cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_GEOID, "invalid"), std::string("invalid"), helper::eqs);
				assert(cr.getProperty("nothing", ""), std::string(""), helper::eqs);
			});

	describe<config::Configuration>("initialize configuration", runner)->test(
			"valid config - full, one feed",
			[]() {
				std::stringstream conf_in;
				conf_in << "[" << SECT_KEY_GENERAL << "]\n" << KV_KEY_FEEDS << "=sens1\n";
				conf_in << KV_KEY_SERVER_PORT << "=1234\n" << KV_KEY_GND_MODE << "=y\n";
				conf_in << "[" << SECT_KEY_FALLBACK << "]\n" << KV_KEY_LATITUDE << "=77.777777\n";
				conf_in << KV_KEY_LONGITUDE << "=-12.121212\n" << KV_KEY_ALTITUDE << "=1234\n";
				conf_in << KV_KEY_GEOID << "=40.4\n" << KV_KEY_PRESSURE << "=999.9\n";
				conf_in << "[" << SECT_KEY_FILTER << "]\n" << KV_KEY_MAX_HEIGHT << "=-1\n";
				conf_in << KV_KEY_MAX_DIST << "=10000\n";
				conf_in << "[sens1]\n" << KV_KEY_HOST << "=localhost\n";
				conf_in << KV_KEY_PORT << "=3456\n" << KV_KEY_PRIORITY << "=1\n";
				config::Configuration config(conf_in);
				auto feed_it = config::Configuration::global_feeds.cbegin();
				assert(feed_it->mName, std::string("sens1"), helper::eqs);
				assert(feed_it->mPriority, 1, helper::eqi);
				assert((std::int32_t) config::Configuration::global_server_port, 1234, helper::eqi);
				assert(config::Configuration::global_gnd_mode, true, helper::eqb);
				assert(config::Configuration::base_latitude, 77.777777, helper::eqd);
				assert(config::Configuration::base_longitude, -12.121212, helper::eqd);
				assert(config::Configuration::base_altitude, 1234, helper::eqi);
				assert(config::Configuration::base_geoid, 40.4, helper::eqd);
				assert(config::Configuration::base_pressure, 999.9, helper::eqd);
				assert(config::Configuration::filter_maxHeight, INT32_MAX, helper::eqi);
				assert(config::Configuration::filter_maxDist, 10000, helper::eqi);
			})->test("only valid feeds",
			[]() {
				config::Configuration::global_feeds.clear();
				std::stringstream conf_in;
				conf_in << "[" << SECT_KEY_GENERAL << "]\n" << KV_KEY_FEEDS << "=sens,sbs1 , sbs2, else,,\n";
				conf_in << "[sens]\n" << KV_KEY_HOST << "=127.0.0.1\n" << KV_KEY_PORT << "=3333\n" << KV_KEY_PRIORITY << "=0\n";
				conf_in << "[sbs1]\n" << KV_KEY_HOST << "=127.0.0.1\n" << KV_KEY_PORT << "=3334\n" << KV_KEY_PRIORITY << "=1\n";
				conf_in << "[sbs2]\n" << KV_KEY_HOST << "=127.0.0.1\n" << KV_KEY_PORT << "=3335\n" << KV_KEY_PRIORITY << "=2\n";
				config::Configuration config(conf_in);
				std::string valid("sens,sbs1,sbs2,");
				std::string result;
				for (auto it = config::Configuration::global_feeds.cbegin(); it != config::Configuration::global_feeds.cend(); it++) {
					result += it->mName + ",";
				}
				assert(result, valid, helper::eqs);
			})->test("no feeds",
			[]() {
				config::Configuration::global_feeds.clear();
				std::stringstream conf_in;
				conf_in << "[" << SECT_KEY_GENERAL << "]\n" << KV_KEY_FEEDS << "=\n";
				assertException<std::logic_error>([&conf_in]() {config::Configuration config(conf_in);});
			});
}
