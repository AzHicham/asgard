#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE direct_path_response_builder_test

#include "asgard/direct_path_response_builder.h"
#include "asgard/request.pb.h"

#include <valhalla/midgard/encoded.h>
#include <valhalla/midgard/pointll.h>
#include <valhalla/proto/trip.pb.h>
#include <valhalla/thor/pathinfo.h>

#include <boost/test/unit_test.hpp>

using namespace valhalla;

namespace asgard {
namespace direct_path_response_builder {

const std::vector<midgard::PointLL> list_geo_points = {
    {50.12345678f, 1.457634f}, {42.46546513f, 8.4646846f}, {49.1036400f, 3.9731065f}, {42.0794687f, 7.974815640f}};

std::vector<thor::PathInfo> create_path_info_list() {
    std::vector<thor::PathInfo> path_info_list;
    for (size_t i = 0; i < 5; ++i) {
        path_info_list.emplace_back(sif::TravelMode::kDrive, i * 5, baldr::GraphId(), 0, i * 10.);
    }
    return path_info_list;
}

valhalla::TripLeg create_trip_leg() {
    auto const s = midgard::encode(list_geo_points);

    valhalla::TripLeg trip_leg;
    trip_leg.set_shape(s);
    for (size_t i = 0; i < list_geo_points.size(); ++i) {
        trip_leg.add_node()->mutable_edge()->set_length((i * 5) / 1000.f);
    }
    return trip_leg;
}

BOOST_AUTO_TEST_CASE(build_journey_response_test) {
    // Empty path_info_list
    {
        pbnavitia::Request request;
        valhalla::TripLeg trip_leg;
        auto response = build_journey_response(request, {}, trip_leg);
        BOOST_CHECK_EQUAL(response.response_type(), pbnavitia::NO_SOLUTION);
        BOOST_CHECK_EQUAL(response.journeys_size(), 0);
    }

    // basic path_info_list (nominal case)
    {
        pbnavitia::Request request;
        std::vector<thor::PathInfo> path_info_list = create_path_info_list();
        request.mutable_direct_path()->set_datetime(1470241573);

        auto trip_leg = create_trip_leg();
        auto response = build_journey_response(request, path_info_list, trip_leg);
        BOOST_CHECK_EQUAL(response.response_type(), pbnavitia::ITINERARY_FOUND);
        BOOST_CHECK_EQUAL(response.journeys_size(), 1);

        auto const journey = response.journeys().begin();
        BOOST_CHECK_EQUAL(journey->nb_sections(), 1);
        BOOST_CHECK_EQUAL(journey->nb_transfers(), 0);
        BOOST_CHECK_EQUAL(journey->duration(), 20);
        BOOST_CHECK_EQUAL(journey->requested_date_time(), 1470241573);
        BOOST_CHECK_EQUAL(journey->departure_date_time(), 1470241573);
        BOOST_CHECK_EQUAL(journey->arrival_date_time(), 1470241593);

        auto const section = journey->sections().begin();
        BOOST_CHECK_EQUAL(section->type(), pbnavitia::STREET_NETWORK);
        BOOST_CHECK_EQUAL(section->id(), "section_0");
        BOOST_CHECK_EQUAL(section->duration(), 20);
        BOOST_CHECK_EQUAL(section->street_network().mode(), pbnavitia::Car);
        BOOST_CHECK_EQUAL(section->begin_date_time(), 1470241573);
        BOOST_CHECK_EQUAL(section->end_date_time(), 1470241593);
        BOOST_CHECK_EQUAL(section->length(), 30);

        auto const origin_coords = section->origin().address().coord();
        BOOST_CHECK_EQUAL(section->origin().uri(), "50.12346;1.45763");
        BOOST_CHECK_EQUAL(section->origin().name(), "");
        BOOST_CHECK_CLOSE(origin_coords.lon(), 50.12345678f, 0.0001f);
        BOOST_CHECK_CLOSE(origin_coords.lat(), 1.457634f, 0.0001f);

        auto const dest_coords = section->destination().address().coord();
        BOOST_CHECK_EQUAL(section->destination().uri(), "42.07947;7.97481");
        BOOST_CHECK_EQUAL(section->destination().name(), "");
        BOOST_CHECK_CLOSE(dest_coords.lon(), 42.0794687f, 0.0001f);
        BOOST_CHECK_CLOSE(dest_coords.lat(), 7.974815640f, 0.0001f);
    }
}

BOOST_AUTO_TEST_CASE(set_extremity_pt_object_test) {
    {
        pbnavitia::Section section;

        set_extremity_pt_object(list_geo_points.front(), section.mutable_origin());
        set_extremity_pt_object(list_geo_points.back(), section.mutable_destination());

        BOOST_CHECK_EQUAL(section.origin().uri(), "50.12346;1.45763");
        BOOST_CHECK_EQUAL(section.origin().name(), "");
        BOOST_CHECK_EQUAL(section.origin().address().coord().lon(), 50.12345678f);
        BOOST_CHECK_EQUAL(section.origin().address().coord().lat(), 1.457634f);

        BOOST_CHECK_EQUAL(section.destination().uri(), "42.07947;7.97482");
        BOOST_CHECK_EQUAL(section.destination().name(), "");
        BOOST_CHECK_EQUAL(section.destination().address().coord().lon(), 42.0794687f);
        BOOST_CHECK_EQUAL(section.destination().address().coord().lat(), 7.974815640f);
    }
}

BOOST_AUTO_TEST_CASE(compute_geojson_test) {
    {
        pbnavitia::Section section;

        compute_geojson(list_geo_points, section);

        BOOST_CHECK_EQUAL(section.mutable_street_network()->coordinates_size(), list_geo_points.size());
        for (size_t i = 0; i < list_geo_points.size(); ++i) {
            const auto coords = section.mutable_street_network()->coordinates(i);
            BOOST_CHECK_EQUAL(coords.lon(), list_geo_points.at(i).lng());
            BOOST_CHECK_EQUAL(coords.lat(), list_geo_points.at(i).lat());
        }
    }
}

void add_section(pbnavitia::Journey& pb_journey, const pbnavitia::SectionType section_type, const pbnavitia::StreetNetworkMode mode,
                 int32_t duration, int32_t length, uint64_t begin_date_time) {
    auto* s = pb_journey.add_sections();
    s->set_type(section_type);
    s->mutable_street_network()->set_mode(mode);
    s->set_id("section");
    s->set_duration(duration);
    s->set_length(length);
    s->set_begin_date_time(begin_date_time);
    s->set_end_date_time(begin_date_time + duration);
}

BOOST_AUTO_TEST_CASE(compute_metadata_test) {
    auto pb_journey = pbnavitia::Journey();
    pb_journey.set_departure_date_time(1470241573);
    pb_journey.set_arrival_date_time(1470242173);

    add_section(pb_journey, pbnavitia::STREET_NETWORK, pbnavitia::StreetNetworkMode::Walking, 1337, 442, 1470241573);
    add_section(pb_journey, pbnavitia::STREET_NETWORK, pbnavitia::StreetNetworkMode::Car, 30, 26, 1470241673);
    add_section(pb_journey, pbnavitia::STREET_NETWORK, pbnavitia::StreetNetworkMode::CarNoPark, 45, 33, 1470241773);
    add_section(pb_journey, pbnavitia::STREET_NETWORK, pbnavitia::StreetNetworkMode::Bike, 62, 777, 1470241873);
    add_section(pb_journey, pbnavitia::STREET_NETWORK, pbnavitia::StreetNetworkMode::Bss, 999, 29, 1470241973);
    add_section(pb_journey, pbnavitia::STREET_NETWORK, pbnavitia::StreetNetworkMode::Ridesharing, 42, 78, 1470242073);
    add_section(pb_journey, pbnavitia::STREET_NETWORK, pbnavitia::StreetNetworkMode::Taxi, 95, 200, 1470242173);

    compute_metadata(pb_journey);

    auto durations = pb_journey.durations();
    auto distances = pb_journey.distances();

    BOOST_CHECK_EQUAL(durations.walking(), 1337);
    BOOST_CHECK_EQUAL(durations.bike(), 1061);
    BOOST_CHECK_EQUAL(durations.car(), 75);
    BOOST_CHECK_EQUAL(durations.ridesharing(), 42);
    BOOST_CHECK_EQUAL(durations.taxi(), 95);

    // Not the sum of all the lengths
    // arrival_last_section - departure_first_section
    // Here (1470242173 + 95) - 1470241573
    BOOST_CHECK_EQUAL(durations.total(), 695);

    BOOST_CHECK_EQUAL(distances.walking(), 442);
    BOOST_CHECK_EQUAL(distances.bike(), 806);
    BOOST_CHECK_EQUAL(distances.car(), 59);
    BOOST_CHECK_EQUAL(distances.ridesharing(), 78);
    BOOST_CHECK_EQUAL(distances.taxi(), 200);
}

BOOST_AUTO_TEST_CASE(compute_path_items_test) {
    // No node in trip_leg
    {
        TripLeg trip_leg;
        auto sn = pbnavitia::StreetNetwork();

        compute_path_items(trip_leg, &sn);

        BOOST_CHECK_EQUAL(sn.path_items_size(), 0);
    }

    // Multiple empty nodes
    {
        TripLeg trip_leg;
        auto sn = pbnavitia::StreetNetwork();
        int nb_nodes = 3;

        // We add 3 nodes but don't set anything
        for (int i = 0; i < nb_nodes; ++i) {
            trip_leg.add_node();
        }

        compute_path_items(trip_leg, &sn);

        BOOST_CHECK_EQUAL(sn.path_items_size(), nb_nodes - 1);
        for (int i = 0; i < nb_nodes - 1; ++i) {
            BOOST_CHECK_EQUAL(sn.path_items().Get(i).has_length(), false);
            BOOST_CHECK_EQUAL(sn.path_items().Get(i).has_name(), false);
            BOOST_CHECK_EQUAL(sn.path_items().Get(i).has_duration(), false);
        }
    }

    // Multiple nodes
    {
        TripLeg trip_leg;
        auto sn = pbnavitia::StreetNetwork();
        auto const nb_nodes = 4;
        auto const lengths_list = std::array<double, nb_nodes>{0., 12., 23., 38.};
        auto const names_list = std::array<std::string, nb_nodes>{"plop", "plip", "plouf"};
        auto const durations_list = std::array<double, nb_nodes>{0., 24., 46., 76.};
        auto const expected_durations_list_in_proto = std::array<double, nb_nodes - 1>{durations_list.at(1) - 0.,
                                                                                       durations_list.at(2) - durations_list.at(1),
                                                                                       durations_list.at(3) - durations_list.at(2)};

        // We add 3 nodes and set all the needed values
        for (int i = 0; i < nb_nodes; ++i) {
            auto* n = trip_leg.add_node();
            n->mutable_edge()->set_length(lengths_list.at(i));
            n->mutable_edge()->add_name()->set_value(names_list.at(i));
            n->set_elapsed_time(durations_list.at(i));
        }

        compute_path_items(trip_leg, &sn);

        // Since we have 4 nodes, we haved 3 edges
        BOOST_CHECK_EQUAL(sn.path_items_size(), nb_nodes - 1);
        for (int i = 0; i < nb_nodes - 1; ++i) {
            BOOST_CHECK_EQUAL(sn.path_items().Get(i).has_length(), true);
            BOOST_CHECK_EQUAL(sn.path_items().Get(i).length(), lengths_list.at(i) * 1000.f);

            BOOST_CHECK_EQUAL(sn.path_items().Get(i).has_name(), true);
            BOOST_CHECK_EQUAL(sn.path_items().Get(i).name(), names_list.at(i));

            BOOST_CHECK_EQUAL(sn.path_items().Get(i).has_duration(), true);
            BOOST_CHECK_EQUAL(sn.path_items().Get(i).duration(), expected_durations_list_in_proto.at(i));
        }
    }
}

BOOST_AUTO_TEST_CASE(set_path_item_name_test) {
    // No value set in path_item
    {
        TripLeg_Node node;
        auto edge = node.mutable_edge();
        auto path_item = pbnavitia::PathItem();

        set_path_item_name(*edge, path_item);
        BOOST_CHECK_EQUAL(path_item.has_name(), false);
    }

    // One value in edge then set in path_item
    {
        TripLeg_Node node;
        auto edge = node.mutable_edge();
        auto path_item = pbnavitia::PathItem();
        auto const value = std::string("Plop");
        edge->mutable_name()->Add()->set_value(value);

        set_path_item_name(*edge, path_item);

        BOOST_CHECK_EQUAL(path_item.has_name(), true);
        BOOST_CHECK_EQUAL(path_item.name(), value);
    }

    // Multiple value in edge, only the first one is set in path_item
    {
        TripLeg_Node node;
        auto edge = node.mutable_edge();
        auto path_item = pbnavitia::PathItem();
        const std::vector<std::string> values_list = {"Plip", "Plouf", "PlapPlap"};
        for (auto const& v : values_list) {
            edge->mutable_name()->Add()->set_value(v);
        }

        set_path_item_name(*edge, path_item);

        BOOST_CHECK_EQUAL(path_item.has_name(), true);
        BOOST_CHECK_EQUAL(path_item.name(), values_list.front());
    }
}

BOOST_AUTO_TEST_CASE(set_path_item_length_test) {
    // No value set in path_item
    {
        TripLeg_Node node;
        auto edge = node.mutable_edge();
        auto path_item = pbnavitia::PathItem();

        set_path_item_length(*edge, path_item);
        BOOST_CHECK_EQUAL(path_item.has_length(), false);
    }

    // One value set in path_item
    {
        TripLeg_Node node;
        auto edge = node.mutable_edge();
        edge->set_length(42.f);
        auto path_item = pbnavitia::PathItem();

        set_path_item_length(*edge, path_item);
        BOOST_CHECK_EQUAL(path_item.has_length(), true);
        BOOST_CHECK_EQUAL(path_item.length(), 42000.f);
    }

    // One value set in path_item
    {
        TripLeg_Node node;
        auto edge = node.mutable_edge();
        edge->set_length(999.f);
        auto path_item = pbnavitia::PathItem();

        set_path_item_length(*edge, path_item);
        BOOST_CHECK_EQUAL(path_item.has_length(), true);
        BOOST_CHECK_EQUAL(path_item.length(), 999000.f);
    }
}

void set_and_check_cycle_path_type(const TripLeg::CycleLane& valhalla_cycle_lane, const pbnavitia::CyclePathType& expected_navitia_type) {
    TripLeg_Node node;
    auto edge = node.mutable_edge();
    edge->set_cycle_lane(valhalla_cycle_lane);
    auto path_item = pbnavitia::PathItem();

    set_path_item_type(*edge, path_item);
    BOOST_CHECK_EQUAL(path_item.has_cycle_path_type(), true);
    BOOST_CHECK_EQUAL(path_item.cycle_path_type(), expected_navitia_type);
}

BOOST_AUTO_TEST_CASE(set_path_item_type_test) {
    // No value set in path_item
    {
        TripLeg_Node node;
        auto edge = node.mutable_edge();
        auto path_item = pbnavitia::PathItem();

        set_path_item_type(*edge, path_item);
        BOOST_CHECK_EQUAL(path_item.cycle_path_type(), false);
    }

    // One value set in path_item
    set_and_check_cycle_path_type(TripLeg_CycleLane_kNoCycleLane, pbnavitia::NoCycleLane);
    set_and_check_cycle_path_type(TripLeg_CycleLane_kShared, pbnavitia::SharedCycleWay);
    set_and_check_cycle_path_type(TripLeg_CycleLane_kDedicated, pbnavitia::DedicatedCycleWay);
    set_and_check_cycle_path_type(TripLeg_CycleLane_kSeparated, pbnavitia::SeparatedCycleWay);
}

// Create a node and set its elapsed_time with node_elapsed_time
// Checks that the value in path_item.duration() is node_elapsed_time - previous
// And set_path_item_duration return value is node_elapsed_time
void exec_set_path_item_duration_test(uint32_t previous, uint32_t node_elapsed_time, uint32_t expected_path_item_duration) {
    TripLeg_Node node;
    node.set_elapsed_time(node_elapsed_time);
    auto path_item = pbnavitia::PathItem();
    auto res = set_path_item_duration(node, previous, path_item);
    BOOST_CHECK_EQUAL(path_item.duration(), expected_path_item_duration);
    BOOST_CHECK_EQUAL(res, node_elapsed_time);
}

BOOST_AUTO_TEST_CASE(set_path_item_duration_test) {
    exec_set_path_item_duration_test(30, 42, 12);
    exec_set_path_item_duration_test(0, 36, 36);
    exec_set_path_item_duration_test(999, 1024, 25);
    exec_set_path_item_duration_test(45, 662, 617);
}

} // namespace direct_path_response_builder
} // namespace asgard
