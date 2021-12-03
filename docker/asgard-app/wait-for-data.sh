#!/bin/bash

valhalla_tiles="/data/valhalla/valhalla_tiles.tar"
config="/data/valhalla/valhalla.json"
elevation_tiles="/data/valhalla/elevation_tiles"

file_status() {
  local file=$1
  if [ -e $file ]  
  then
    echo "$file is OK"		
  else
    echo "still waiting for $file"		
  fi
}

until [ -e $valhalla_tiles ] && [ -e $config ] && [ -e $elevation_tiles ]
do
  date
  file_status $elevation_tiles
  file_status $config
  file_status $valhalla_tiles
  sleep 5
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
