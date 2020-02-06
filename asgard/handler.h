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

#pragma once

#include "asgard/mode_costing.h"
#include "asgard/response.pb.h"

#include <valhalla/baldr/graphreader.h>
#include <valhalla/thor/bidirectional_astar.h>
#include <valhalla/thor/timedistancematrix.h>

namespace pbnavitia {
class Request;
}

namespace asgard {

struct Context;
class Metrics;
class Projector;

struct Handler {
    explicit Handler(const Context&);
    pbnavitia::Response handle(const pbnavitia::Request&);

private:
    pbnavitia::Response handle_matrix(const pbnavitia::Request&);
    pbnavitia::Response handle_direct_path(const pbnavitia::Request&);

    valhalla::thor::PathAlgorithm& get_path_algorithm(const std::string& mode);

    valhalla::baldr::GraphReader& graph;
    valhalla::thor::TimeDistanceMatrix matrix;
    valhalla::thor::BidirectionalAStar bda;
    valhalla::thor::AStarPathAlgorithm astar;
    ModeCosting mode_costing;
    const Metrics& metrics;
    const Projector& projector;
};

} // namespace asgard
