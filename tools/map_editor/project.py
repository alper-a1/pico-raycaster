from PySide6.QtCore import QObject, Signal
from pathlib import Path
import logging

from map_data import MapData, PlayerData
from xip_writer import export_map_to_xip

class Project(QObject):
    map_changed = Signal()
    map_loaded = Signal()
    dirty_changed = Signal(bool)  # dirty flag changed
    map_saved = Signal(str) # error message or empty if success
    
    def __init__(self, assets_path: Path):
        super().__init__()
        self.map: MapData = MapData(0, 0, [])
        self.player: PlayerData = PlayerData(0, 0, 0, 0)
        
        self._assets_path = assets_path
        # unsaved changes flag
        self._dirty = False
        
    @property
    def dirty(self) -> bool:
        return self._dirty
    
    @dirty.setter
    def dirty(self, value: bool):
        self._dirty = value
        self.dirty_changed.emit(value)
        
    def new_map(self, width: int, height: int):
        logging.info(f"Creating new map with size {width}x{height}")
        
        # new empty map 
        self.map = MapData(width=width, height=height, tiles=[0] * (width * height))
        self.dirty = True
        self.map_loaded.emit()
        
    def in_bounds(self, x: int, y: int) -> bool:
        if self.map is None:
            return False
        return 0 <= x < self.map.width and 0 <= y < self.map.height
    
    def _coord_to_index(self, x: int, y: int) -> int:
        assert self.map is not None
        return y + self.map.height * x  # col major storage
    
    def get_tile(self, x: int, y: int) -> int:
        if self.map is None or not self.in_bounds(x, y):
            return -1
        return self.map.tiles[self._coord_to_index(x, y)] 
        
    def set_tile(self, x: int, y: int, tile_id: int):
        if self.map is None or not self.in_bounds(x, y):
            return
        
        index = self._coord_to_index(x, y)
        self.map.tiles[index] = tile_id
        
        self.dirty = True
        self.map_changed.emit()
        
        logging.info(f"Set tile at {x}, {y} to {tile_id}")
        
    def set_player_start_pos(self, start_x: int, start_y: int):
        """
        Set player start position in FixedPoint15_16 format.
        """
        self.player.start_x = start_x
        self.player.start_y = start_y
    
        self.dirty = True
        
    def set_player_start_angle(self, start_angle_x: int, start_angle_y: int):
        """
        Set player start angle in FixedPoint15_16 format.
        """
        
        self.player.start_angle_x = start_angle_x
        self.player.start_angle_y = start_angle_y
    
        self.dirty = True
        
    def save_map(self, filename: str):
        if self.map is None or self.player is None:
            self.map_saved.emit("No map or player data to save.")
            return
        
        export_map_to_xip(Path.joinpath(self._assets_path, filename), self.player, self.map)
        
        self.dirty = False
        
        self.map_saved.emit("")  # success
        
    