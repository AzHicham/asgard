#!/bin/sh

usage() { echo "Usage: $0 -e <path_to_valhalla_executables> -i <path_of_pbf_dir> -o <path_of_output_dir>" 1>&2; exit 1; }

while getopts ":e:i:o:" option; do
    case "${option}" in
        e)
            executable_path=`realpath ${OPTARG}`
            ;;
        i)
            input_dir=`realpath ${OPTARG}`
            ;;
        o)
            output_dir=`realpath ${OPTARG}`
            ;;
    esac
done

shift $((OPTIND-1))
if [ -z "${executable_path}" ] || [ -z "${input_dir}" ] || [ -z "${output_dir}" ]; then
    echo "Bad parameters. Doing Nothing."
    usage
    exit 1
fi

echo "executable_path = ${executable_path}"
echo "input_dir = ${input_dir}"
echo "output_dir = ${output_dir}"

# Download the pbf given in argument
cd ${output_dir}
mkdir -p valhalla_tiles 
${executable_path}/valhalla_build_config --mjolnir-tile-dir ${PWD}/valhalla_tiles --mjolnir-tile-extract ${PWD}/valhalla_tiles.tar --mjolnir-timezone ${PWD}/valhalla_tiles/timezones.sqlite --mjolnir-admin ${PWD}/valhalla_tiles/admins.sqlite > valhalla.json 
# These are default values set after empirically testing asgard with distributed.
# They seem to give the most coherent results between the time of the matrix and the direct_path
# And fixing some projection problems in dead-ends or isolated places.
sed -i 's,\"minimum_reachability\"\: [[:digit:]]*,\"minimum_reachability\"\: 0,' valhalla.json
sed -i 's,\"radius\"\: [[:digit:]]*,\"radius\"\: 30,' valhalla.json
sed -i 's,\"shortcuts\"\: [^,]*,\"shortcuts\"\: false,' valhalla.json
sed -i 's,\"hierarchy\"\: [^,]*,\"hierarchy\"\: false,' valhalla.json
${executable_path}/valhalla_build_tiles -c valhalla.json ${input_dir}/*.pbf
find valhalla_tiles | sort -n | tar cf valhalla_tiles.tar --no-recursion -T -
# Make the graph shareable between threads in a thread-safe way
# This has to be set after the creation of the tiles to avoid seg faults
sed -i '/"hierarchy":/a \\t"global_synchronized_cache": true,' valhalla.json

