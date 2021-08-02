/* Copyright © 2001-2014, Canal TP and/or its affiliates. All rights reserved.

This file is part of Navitia,
    the software to build cool stuff with public transport.

Hope you'll enjoy and contribute to this project,
    powered by Canal TP (www.canaltp.fr).
Help us simplify mobility and open public transport:
    a non ending quest to the responsive locomotion way of traveling!

LICENCE: This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Stay tuned using
twitter @navitia
IRC #navitia on freenode
// Copyright 2017-2018, CanalTP and/or its affiliates. All rights reserved.
//
// LICENCE: This program is free software; you can redistribute it
// and/or modify it under the terms of the GNU Affero General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program. If not, see
// <http://www.gnu.org/licenses/>.



/**
 * Provide facilities to initialize an app
 *
 * Initialize signal handling
 */

#pragma once

#include "utils/backtrace.h"

#include <valhalla/midgard/logging.h>

#include <unistd.h>
#include <csignal>
#include <iostream>

namespace asgard {

inline void print_backtrace() {
    LOG_ERROR(navitia::get_backtrace());

    // without the flush we might not have the error in the standard output
    std::cout.flush();
    std::cerr.flush();
}

void before_dying(int signum) {
    LOG_ERROR("We received signal: " + std::to_string(signum) + ", so it's time to die!! ");

    print_backtrace();

    signal(signum, SIG_DFL);
    kill(getpid(), signum); // kill the process to enable the core generation
}

void before_exit(int signum) {
    LOG_INFO("Process terminated by signal: " + std::to_string(signum));

    signal(signum, SIG_DFL);
    exit(0);
}

inline void init_signal_handling() {
    signal(SIGPIPE, before_dying);
    signal(SIGABRT, before_dying);
    signal(SIGSEGV, before_dying);
    signal(SIGFPE, before_dying);
    signal(SIGILL, before_dying);
    signal(SIGKILL, before_dying);

    signal(SIGTERM, before_exit);
    signal(SIGINT, before_exit);
}

inline void init_app() {
    init_signal_handling();
}

} // namespace asgard
