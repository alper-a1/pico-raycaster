"""Module to export map and player data to a custom .xip binary format.

See xip layout specification below:

HEADER:
- MAGIC (4 bytes): b'MAP0'
- VERSION (4 bytes): uint32
- PLAYERDATA OFFSET (4 bytes): uint32
- MAPDATA OFFSET (4 bytes): uint32
- RESERVED (4 bytes): zeroed

PLAYERDATA:
- start_x (4 bytes): FixedPoint15_16 (int32)
- start_y (4 bytes): FixedPoint15_16 (int32)
- start_angle_x (4 bytes): FixedPoint15_16 (int32)
- start_angle_y (4 bytes): FixedPoint15_16 (int32)

MAPDATA:
- width (1 byte): uint8
- height (1 byte): uint8
- tiles (width * height bytes): uint8 tile indices (stored col-major)
"""

import struct
from pathlib import Path
import logging

from map_data import PlayerData, MapData

MAP_VERSION = 100000 # version 1.0.00
MAP_MAGIC = b'MAP0'
MAP_MAGIC_BYTES = 0x4D415030


def export_map_to_xip(filename: Path, player_data: PlayerData, map_data: MapData):
    try:
        with open(filename, "wb") as f:   
            # Write header
            f.write(MAP_MAGIC)  # MAGIC
            f.write(struct.pack('<I', MAP_VERSION))  # VERSION
            
            playerdata_offset = 20  # after header
            mapdata_offset = playerdata_offset + 16  # after player data
            
            f.write(struct.pack('<I', playerdata_offset))  # PLAYERDATA OFFSET
            f.write(struct.pack('<I', mapdata_offset))  # MAPDATA OFFSET
            f.write(b'\x00' * 4)  # RESERVED
            
            # Write player data
            f.write(struct.pack('<i', player_data.start_x))
            f.write(struct.pack('<i', player_data.start_y))
            f.write(struct.pack('<i', player_data.start_angle_x))
            f.write(struct.pack('<i', player_data.start_angle_y))
            
            # Write map data
            f.write(struct.pack('<B', map_data.width))
            f.write(struct.pack('<B', map_data.height))
            f.write(struct.pack("<" + "B" * (map_data.width * map_data.height), *map_data.tiles))
    except Exception as e:
        logging.error(f"Error exporting map to {filename}: {e}")