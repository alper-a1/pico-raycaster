/**
 * @file map_data.hpp
 */

#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <cstdint>

#include "fixed_point.hpp"

struct MapFileHeader {
    inline static constexpr uint32_t VALID_MAGIC = 0x3050414D; // 'MAP0' reversed for little endian

    uint32_t magic;
    uint32_t version;
    uint32_t playerdata_offset;
    uint32_t mapdata_offset;
    uint32_t reserved;
};

extern "C" {
    // defined in assets_bin/map_data.S
    // kept as byte array for pointer math
    extern const uint8_t map_data_xip_blob[]; 
    extern const uint8_t map_data_xip_blob_end[];
}

struct PlayerData {
    Fixed15_16 pos_x;
    Fixed15_16 pos_y;
    Fixed15_16 dir_x;
    Fixed15_16 dir_y;
};

struct MapView {
    const uint8_t width;
    const uint8_t height;

    const uint8_t* const tile_data;

    /**
     * @brief Construct a MapView
     * @param w Width of the map in tiles
     * @param h Height of the map in tiles
     * @param tile_data Pointer to the tile data array (column major)
     */
    MapView(uint8_t w, uint8_t h, const uint8_t* tile_data) : width(w), height(h), tile_data(tile_data) {}

    /// @brief Get the tile at (x, y) with bounds checking
    inline uint8_t getTile(uint8_t x, uint8_t y) const {
        if (x >= width || y >= height) {
            return 0; // out of bounds
        }
        return getTileUnchecked(x, y); // column major
    }

    /// @brief Get the tile at (x, y) WITHOUT bounds checking
    inline uint8_t getTileUnchecked(uint8_t x, uint8_t y) const {
        return tile_data[y + height * x]; // column major
    }
};

bool isMapDataValid();

const MapFileHeader* getMapFileHeader();

/// @note assumes map file data is valid
const PlayerData* getPlayerData();

/// @note assumes map file data is valid
MapView createMapView();

#endif // MAP_DATA_H