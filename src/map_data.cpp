/**
 * @file map_data.cpp
 */

#include "map_data.hpp"

const MapFileHeader* getMapFileHeader() {
    return reinterpret_cast<const MapFileHeader*>(map_data_xip_blob);
}

bool isMapDataValid() {
    const MapFileHeader* header = getMapFileHeader();
    
    return (header->magic == MapFileHeader::VALID_MAGIC);
}

const PlayerData* getPlayerData() {
    const MapFileHeader* header = getMapFileHeader();

    return reinterpret_cast<const PlayerData*>(map_data_xip_blob + header->playerdata_offset);
}

MapView createMapView() {
    const MapFileHeader* header = getMapFileHeader();
    
    const uint8_t* map_data_ptr = map_data_xip_blob + header->mapdata_offset;

    // first two bytes are width and height
    uint8_t width = map_data_ptr[0];
    uint8_t height = map_data_ptr[1];

    // tile data starts after width and height bytes
    const uint8_t* tile_data = &map_data_ptr[2];

    return MapView(width, height, tile_data);
}
