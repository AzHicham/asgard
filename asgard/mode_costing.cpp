#include "asgard/mode_costing.h"
#include "asgard/request.pb.h"
#include "asgard/util.h"

#include <valhalla/proto/options.pb.h>
#include <valhalla/sif/costconstants.h>

using namespace valhalla;
using vc = valhalla::Costing;

namespace asgard {

namespace {

Options make_default_directions_options(const std::string& mode) {
    Options default_directions_options;

    valhalla::Costing costing = util::convert_navitia_to_valhalla_costing(mode);
    default_directions_options.set_costing(costing);

    sif::ParseCostingOptions(rapidjson::Document{}, "", default_directions_options);

    return default_directions_options;
}

std::string to_valhalla_bicycle_type(const pbnavitia::BicycleType type) {
    switch (type) {
    case pbnavitia::BicycleType::cross:
        return "Cross";
    case pbnavitia::BicycleType::road:
        return "Road";
    case pbnavitia::BicycleType::moutain:
        return "Mountain";
    default:
        return "Hybrid";
    }
}

Options make_costing_option(const ModeCostingArgs& args) {

    Options options = make_default_directions_options(args.mode);
    // walking
    options.mutable_costing_options()->at(vc::pedestrian).set_walking_speed(args.speeds[vc::pedestrian] * 3.6);

    // bike
    options.mutable_costing_options()->at(vc::bicycle).set_cycling_speed(args.speeds[vc::bicycle] * 3.6);
    options.mutable_costing_options()->at(vc::bicycle).set_use_roads(args.bike_use_roads);
    options.mutable_costing_options()->at(vc::bicycle).set_use_hills(args.bike_use_hills);
    options.mutable_costing_options()->at(vc::bicycle).set_use_ferry(args.bike_use_ferry);
    options.mutable_costing_options()->at(vc::bicycle).set_avoid_bad_surfaces(args.bike_avoid_bad_surfaces);
    options.mutable_costing_options()->at(vc::bicycle).set_shortest(args.bike_shortest);
    options.mutable_costing_options()->at(vc::bicycle).set_transport_type(to_valhalla_bicycle_type(args.bicycle_type));
    options.mutable_costing_options()->at(vc::bicycle).set_use_living_streets(args.bike_use_living_streets);
    options.mutable_costing_options()->at(vc::bicycle).set_maneuver_penalty(args.bike_maneuver_penalty);
    options.mutable_costing_options()->at(vc::bicycle).set_service_penalty(args.bike_service_penalty);
    options.mutable_costing_options()->at(vc::bicycle).set_service_factor(args.bike_service_factor);
    options.mutable_costing_options()->at(vc::bicycle).set_country_crossing_cost(args.bike_country_crossing_cost);
    options.mutable_costing_options()->at(vc::bicycle).set_country_crossing_penalty(args.bike_country_crossing_penalty);

    // rent cost
    options.mutable_costing_options()->at(vc::pedestrian).set_bike_share_cost(args.bss_return_duration);
    options.mutable_costing_options()->at(vc::pedestrian).set_bike_share_penalty(args.bss_return_penalty);

    // return cost
    options.mutable_costing_options()->at(vc::bicycle).set_bike_share_cost(args.bss_rent_duration);
    options.mutable_costing_options()->at(vc::bicycle).set_bike_share_penalty(args.bss_rent_penalty);

    return options;
}
} // namespace

ModeCosting::ModeCosting() {
    Options options;
    rapidjson::Document doc;
    sif::ParseCostingOptions(doc, "/costing_options", options);
    for (const auto& mode : {"car", "taxi", "walking", "bike"}) {
        costing[util::navitia_to_valhalla_mode_index(mode)] = factory.Create(options.costing_options().at(util::convert_navitia_to_valhalla_costing(mode)));
    }
}

void ModeCosting::update_costing(const ModeCostingArgs& args) {
    auto new_options = make_costing_option(args);
    auto travel_mode = util::convert_navitia_to_valhalla_mode(args.mode);
    costing = factory.CreateModeCosting(new_options, travel_mode);
}

const valhalla::sif::cost_ptr_t ModeCosting::get_costing_for_mode(const std::string& mode) const {
    return costing[util::navitia_to_valhalla_mode_index(mode)];
}

} // namespace asgard
