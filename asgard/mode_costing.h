#pragma once

#include <asgard/type.pb.h>
#include <valhalla/sif/costfactory.h>
#include <valhalla/sif/dynamiccost.h>

namespace asgard {

static const size_t mode_costing_size = static_cast<size_t>(valhalla::sif::TravelMode::kMaxTravelMode);

using Costing = valhalla::sif::mode_costing_t;

struct ModeCostingArgs {
    std::string mode = "";
    std::array<float, valhalla::Costing_MAX> speeds;
    // all costs and penalties are in seconds
    float bss_rent_duration = 120;
    float bss_rent_penalty = 0;
    float bss_return_duration = 120;
    float bss_return_penalty = 0;

    // Bike
    float bike_use_roads = 0.5;
    float bike_use_hills = 0.5;
    float bike_use_ferry = 0.5;
    float bike_avoid_bad_surfaces = 0.25;
    bool bike_shortest = false;
    pbnavitia::BicycleType bicycle_type = pbnavitia::hybrid;
    float bike_use_living_streets = 0.5;
    float bike_maneuver_penalty = 5;
    float bike_service_penalty = 0;
    float bike_service_factor = 1;
    float bike_country_crossing_cost = 600;
    float bike_country_crossing_penalty = 0;

    ModeCostingArgs() {
        mode = "";
        speeds.fill(0);
        bss_rent_duration = 120;
        bss_rent_penalty = 0;
        bss_return_duration = 120;
        bss_return_penalty = 0;

        bike_use_roads = 0.5;
        bike_use_hills = 0.5;
        bike_use_ferry = 0.5;
        bike_avoid_bad_surfaces = 0.25;
        bike_shortest = false;
        bicycle_type = pbnavitia::hybrid;
        bike_use_living_streets = 0.5;
        bike_maneuver_penalty = 5;
        bike_service_penalty = 0;
        bike_service_factor = 1;
        bike_country_crossing_cost = 600;
        bike_country_crossing_penalty = 0;
    }
};

class ModeCosting {
public:
    ModeCosting();
    ModeCosting(const ModeCosting&) = delete;

    void update_costing(const ModeCostingArgs& args);

    const valhalla::sif::cost_ptr_t get_costing_for_mode(const std::string& mode) const;

    const Costing& get_costing() const { return costing; }

private:
    valhalla::sif::CostFactory factory;
    Costing costing;
};

} // namespace asgard
