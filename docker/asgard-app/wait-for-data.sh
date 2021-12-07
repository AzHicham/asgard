#!/bin/bash

valhalla_tiles="/data/valhalla/valhalla_tiles.tar"
config="/data/valhalla/valhalla.json"
elevation_tiles="/data/valhalla/elevation_tiles"
healthcheck="/data/valhalla/healthcheck.sh"

file_status() {
  local file=$1
  if [ -e $file ]  
  then
    echo "$file is OK"		
  else
    echo "still waiting for $file"		
  fi
}

until [ -e $valhalla_tiles ] && [ -e $config ] && [ -e $elevation_tiles ] && [-e healthcheck]
do
  date
  file_status $elevation_tiles
  file_status $config
  file_status $valhalla_tiles
  file_status $healthcheck
  sleep 5
done
  echo "file  $valhalla_tiles, $config, $elevation_tiles and $healthcheck OK !!"

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

is_ready $valhalla_tiles && is_ready $config && is_ready $elevation_tiles && is_ready $healthcheck
