#pragma once

#include "utils/coord_parser.h"
#include "asgard/response.pb.h"

#include <valhalla/midgard/pointll.h>
#include <valhalla/proto/directions.pb.h>
#include <valhalla/proto/options.pb.h>
#include <valhalla/proto/trip.pb.h>

#include <boost/range/algorithm/transform.hpp>

namespace valhalla {
namespace sif {
enum class TravelMode : uint8_t;
}
} // namespace valhalla

namespace asgard {

namespace util {

pbnavitia::StreetNetworkMode convert_valhalla_to_navitia_mode(const valhalla::sif::TravelMode& mode);
pbnavitia::StreetNetworkMode convert_valhalla_to_navitia_mode(const valhalla::TravelMode& mode);

valhalla::sif::TravelMode convert_navitia_to_valhalla_mode(const std::string& mode);

size_t navitia_to_valhalla_mode_index(const std::string& mode);

valhalla::Costing convert_navitia_to_valhalla_costing(const std::string& costing);

pbnavitia::CyclePathType convert_valhalla_to_navitia_cycle_lane(const valhalla::TripLeg_CycleLane& cycle_lane);

pbnavitia::StreetNetworkMode convert_navitia_to_streetnetwork_mode(const std::string& mode);

template<typename SingleRange>
std::vector<valhalla::midgard::PointLL> convert_locations_to_pointLL(const SingleRange& request_locations) {
    std::vector<valhalla::midgard::PointLL> points;
    points.reserve(request_locations.size());

    boost::transform(request_locations, std::back_inserter(points),
                     [](const auto& l) {
                         if (l.has_lat() && l.has_lon()) {
                             return valhalla::midgard::PointLL{l.lon(), l.lat()};
                         } else {
                             std::pair<double, double> coords = navitia::parse_coordinate(l.place());
                             return valhalla::midgard::PointLL{coords.first, coords.second};
                         }
                     });

    return points;
}

} // namespace util

} // namespace asgard
