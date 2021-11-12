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

#include "tile_maker.h"

#include <valhalla/baldr/graphconstants.h>
#include <valhalla/mjolnir/directededgebuilder.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

namespace asgard {

namespace tile_maker {

/*

  The graph looks like this
                           F-->--<--
    ___G____              /        | _____H_____
   /        \            /         |/           \
  A--->--<---B--->--<---C--->--<---D----->---<---I
              \                    |
               \                   |
                E------>--<--------|

 */

void TileMaker::make_tile() {
    using namespace valhalla::mjolnir;
    using namespace valhalla::baldr;

    // make sure that all the old tiles are gone before trying to make new ones.
    if (boost::filesystem::is_directory(tile_dir)) {
        boost::filesystem::remove_all(tile_dir);
    }

    GraphTileBuilder tile(tile_dir, tile_id, false);

    // Set the base lat,lon of the tile
    uint32_t id = tile_id.tileid();
    const auto& tl = TileHierarchy::levels().rbegin();
    PointLL base_ll = tl->tiles.Base(id);
    tile.header_builder().set_base_ll(base_ll);

    uint32_t edge_index = 0;

    auto add_node = [&](const std::pair<GraphId, PointLL>& v,
                        const uint32_t edge_count,
                        const uint16_t access = kAllAccess,
                        const bool is_bss_node = false) {
        NodeInfo node_builder;
        node_builder.set_latlng(base_ll, v.second);
        // node_builder.set_road_class(RoadClass::kSecondary);
        node_builder.set_access(access);
        node_builder.set_edge_count(edge_count);
        node_builder.set_edge_index(edge_index);
        node_builder.set_timezone(1);
        if (is_bss_node) {
            node_builder.set_type(valhalla::baldr::NodeType::kBikeShare);
        }

        edge_index += edge_count;
        tile.nodes().emplace_back(node_builder);
    };

    auto add_edge = [&](const std::pair<GraphId, PointLL>& u,
                        const std::pair<GraphId, PointLL>& v,
                        const uint32_t localedgeidx,
                        const uint32_t opposing_edge_index,
                        const bool forward,
                        const uint16_t forward_access = kAllAccess,
                        const uint16_t backward_access = kAllAccess,
                        const bool is_bss_connection = false) {
        DirectedEdgeBuilder edge_builder({}, v.first, forward, u.second.Distance(v.second), 1, 1,
                                         Use::kRoad, RoadClass::kMotorway, localedgeidx,
                                         false, 0, 0, false, 0, 0, false);
        edge_builder.set_opp_index(opposing_edge_index);
        edge_builder.set_forwardaccess(forward_access);
        edge_builder.set_reverseaccess(backward_access);
        edge_builder.set_free_flow_speed(100);
        edge_builder.set_constrained_flow_speed(10);
        edge_builder.set_bss_connection(is_bss_connection);

        std::vector<PointLL> shape = {u.second, u.second.PointAlongSegment(v.second), v.second};
        if (!forward) {
            std::reverse(shape.begin(), shape.end());
        }
        bool added;
        // make more complex edge geom so that there are 3 segments, affine combination doesnt properly
        // handle arcs but who cares
        uint32_t edge_info_offset = tile.AddEdgeInfo(localedgeidx, u.first, v.first, 123, // way_id
                                                     0, 0,
                                                     120, // speed limit in kph
                                                     shape,
                                                     {std::to_string(localedgeidx)},
                                                     {},
                                                     0,
                                                     added);
        // assert(added);
        edge_builder.set_edgeinfo_offset(edge_info_offset);
        tile.directededges().emplace_back(edge_builder);
    };

    add_edge(a, b, 0, 0, true);
    // G is bike share station
    add_edge(a, g, 1, 0, true, (kPedestrianAccess | kBicycleAccess), true);
    add_node(a, 2);

    add_edge(b, a, 2, 0, false);
    add_edge(b, c, 3, 0, true);
    add_edge(b, e, 4, 0, true);
    add_edge(b, g, 5, 1, true, (kPedestrianAccess | kBicycleAccess), true);
    add_node(b, 4);

    add_edge(c, b, 6, 1, false);
    add_edge(c, d, 7, 0, true);
    add_edge(c, f, 8, 0, true);
    add_node(c, 3);

    add_edge(d, c, 9, 1, false);
    add_edge(d, e, 10, 1, false);
    add_edge(d, f, 11, 1, false);
    add_edge(d, i, 12, 0, false);
    add_edge(d, h, 13, 0, true, (kPedestrianAccess | kBicycleAccess), true);
    add_node(d, 5);

    add_edge(e, b, 14, 2, false);
    add_edge(e, d, 15, 1, true);
    add_node(e, 2);

    add_edge(f, c, 16, 2, false);
    add_edge(f, d, 17, 2, true);
    add_node(f, 2);

    // Bike Share station
    add_edge(g, a, 18, 1, false, (kPedestrianAccess | kBicycleAccess), true);
    add_edge(g, b, 19, 3, false, (kPedestrianAccess | kBicycleAccess), true);
    add_node(g, 2, (kPedestrianAccess | kBicycleAccess), true);

    // Bike Share station
    add_edge(h, d, 20, 4, false, (kPedestrianAccess | kBicycleAccess), true);
    add_edge(h, i, 21, 1, false, (kPedestrianAccess | kBicycleAccess), true);
    add_node(h, 2, (kPedestrianAccess | kBicycleAccess), true);

    add_edge(i, d, 22, 3, false);
    add_edge(i, h, 23, 1, true, (kPedestrianAccess | kBicycleAccess), true);
    add_node(i, 2);

    tile.StoreTileData();

    // write the bin data
    GraphTileBuilder::tweeners_t tweeners;
    auto reloaded = GraphTile::Create(tile_dir, tile_id);
    auto bins = GraphTileBuilder::BinEdges(reloaded, tweeners);
    GraphTileBuilder::AddBins(tile_dir, reloaded, bins);
}

} // namespace tile_maker

} // namespace asgard
