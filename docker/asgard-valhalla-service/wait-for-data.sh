#!/bin/bash

valhalla_tiles="/data/valhalla/valhalla_tiles.tar"
config="/data/valhalla/valhalla.json"
elevation_tiles="/data/valhalla/elevation_tiles"

until [ -e $valhalla_tiles ] && [ -e $config ] && [ -e $elevation_tiles ]
do
        echo "file $valhalla_tiles, $config or $elevation_tiles not exists"
        sleep 2
done
        echo "file  $valhalla_tiles, $config and $elevation_tiles  OK !!"

is_ready () {
  local file=$1
  currentSeconds=$(date +%s)
  lastModificationSeconds=$(date +%s -r $file)

  until [ "$(($currentSeconds - $lastModificationSeconds))" -gt "4" ]
  do
    echo -e "Last modification:$(($currentSeconds - $lastModificationSeconds)) \nfile $file is in use, waiting 2s ..."
    #if the file is still used, we wait
    sleep 2
    currentSeconds=$(date +%s)
    lastModificationSeconds=$(date +%s -r $file)
  done
  echo "File $1 Ready"
}

is_ready $valhalla_tiles && is_ready $config && is_ready $elevation_tiles