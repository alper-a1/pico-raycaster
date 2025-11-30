from dataclasses import dataclass

@dataclass
class PlayerData:
    start_x: int  # FixedPoint15_16
    start_y: int  # FixedPoint15_16
    start_angle_x: int  # FixedPoint15_16
    start_angle_y: int  # FixedPoint15_16
    
@dataclass
class MapData:
    width: int  # uint8
    height: int  # uint8
    tiles: list[int]  # list of uint8 tile indices (col-major)
    