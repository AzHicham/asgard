#include "util.h"

#include <valhalla/midgard/logging.h>
#include <valhalla/sif/costconstants.h>

using namespace valhalla;

namespace asgard {

namespace util {

pbnavitia::StreetNetworkMode convert_valhalla_to_navitia_mode(const sif::TravelMode& mode) {
    switch (mode) {
    case sif::TravelMode::kDrive:
        return pbnavitia::Car;

    case sif::TravelMode::kPedestrian:
        return pbnavitia::Walking;

    case sif::TravelMode::kBicycle:
        return pbnavitia::Bike;

    default:
        throw std::invalid_argument("Bad convert_valhalla_to_navitia_mode parameter");
    }
}

pbnavitia::StreetNetworkMode convert_valhalla_to_navitia_mode(const valhalla::TravelMode& mode) {
    switch (mode) {
    case valhalla::TravelMode::kDrive:
        return pbnavitia::Car;

    case valhalla::TravelMode::kPedestrian:
        return pbnavitia::Walking;

    case valhalla::TravelMode::kBicycle:
        return pbnavitia::Bike;

    default:
        throw std::invalid_argument("Bad convert_valhalla_to_navitia_mode parameter");
    }
}

const std::map<std::string, sif::TravelMode> make_navitia_to_valhalla_mode_map() {
    return {
        {"walking", sif::TravelMode::kPedestrian},
        {"bike", sif::TravelMode::kBicycle},
        {"car", sif::TravelMode::kDrive},
        {"taxi", sif::TravelMode::kDrive},
        {"bss", sif::TravelMode::kPedestrian},
    };
}

sif::TravelMode convert_navitia_to_valhalla_mode(const std::string& mode) {
    static const auto navitia_to_valhalla_mode_map = make_navitia_to_valhalla_mode_map();
    return navitia_to_valhalla_mode_map.at(mode);
}

const std::map<std::string, pbnavitia::StreetNetworkMode> make_navitia_to_streetnetwork_mode(const std::string& mode) {
    return {
        {"walking", pbnavitia::StreetNetworkMode::Walking},
        {"bike", pbnavitia::StreetNetworkMode::Bike},
        {"car", pbnavitia::StreetNetworkMode::Car},
        {"taxi", pbnavitia::StreetNetworkMode::Taxi},
        {"bss", pbnavitia::StreetNetworkMode::Bss},
    };
}

pbnavitia::StreetNetworkMode convert_navitia_to_streetnetwork_mode(const std::string& mode) {
    static const auto navitia_to_valhalla_mode_map = make_navitia_to_streetnetwork_mode(mode);
    return navitia_to_valhalla_mode_map.at(mode);
}

size_t navitia_to_valhalla_mode_index(const std::string& mode) {
    static const auto navitia_to_valhalla_mode_map = make_navitia_to_valhalla_mode_map();
    return static_cast<size_t>(navitia_to_valhalla_mode_map.at(mode));
}

const std::map<std::string, Costing> make_navitia_to_valhalla_costing_map() {
    return {
        {"walking", Costing::pedestrian},
        {"bike", Costing::bicycle},
        {"car", Costing::auto_},
        {"taxi", Costing::taxi},
        {"bss", Costing::bikeshare},
    };
}

Costing convert_navitia_to_valhalla_costing(const std::string& costing) {
    static const auto navitia_to_valhalla_costing_map = make_navitia_to_valhalla_costing_map();
    return navitia_to_valhalla_costing_map.at(costing);
}

pbnavitia::CyclePathType convert_valhalla_to_navitia_cycle_lane(const TripLeg_CycleLane& cycle_lane) {
    switch (cycle_lane) {
    case TripLeg_CycleLane_kNoCycleLane:
        return pbnavitia::NoCycleLane;

    case TripLeg_CycleLane_kShared:
        return pbnavitia::SharedCycleWay;

    case TripLeg_CycleLane_kDedicated:
        return pbnavitia::DedicatedCycleWay;

    case TripLeg_CycleLane_kSeparated:
        return pbnavitia::SeparatedCycleWay;

    default:
        LOG_WARN("Unknown convert_valhalla_to_navitia_cycle_lane parameter. Value = " + std::to_string(cycle_lane));
        return pbnavitia::NoCycleLane;
    }
}

} // namespace util

} // namespace asgard
