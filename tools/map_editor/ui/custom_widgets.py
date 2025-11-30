import logging

from PySide6 import QtCore, QtGui
from PySide6.QtCore import Qt, Signal

from PySide6.QtWidgets import (QLabel, QCheckBox, QPushButton, QSlider,
                               QVBoxLayout, QHBoxLayout, QWidget, QDialog,
                               QDialogButtonBox, QInputDialog)

from project import Project # only for type hinting, no direct usage

class TileListManagerWidget(QWidget):
    tile_selected = Signal(int)  # tile_id
    
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout(self)
        layout.addWidget(QLabel("Tile List Manager (TODO)"))
        # TODO implement tile list management UI


class MapCanvasWidget(QWidget):
    tile_drawn = Signal(int, int, int)  # x, y, tile_id
    
    def __init__(self, tile_draw_size: int = 16):
        super().__init__()
        self.setFixedSize(512, 512)
        
        self._tile_draw_size = tile_draw_size
        
        self._project: Project | None = None
    
    def set_project(self, project: Project):
        self._project = project
        if project is not None:
            # project.tile_changed.connect(self.on_tile_changed)
            project.map_loaded.connect(self.on_map_loaded)
        
        self.repaint()
        
    def on_map_loaded(self):
        if self._project is None:
            return
        
        # TODO scale tile size to fit on screen if large map 
        
        width = self._project.map.width * self._tile_draw_size
        height = self._project.map.height * self._tile_draw_size
        self.setFixedSize(width, height)
        
        self.resize(width, height)
        self.repaint()
        
    def paintEvent(self, event: QtGui.QPaintEvent) -> None:
        painter = QtGui.QPainter(self)
        pen = QtGui.QPen(Qt.GlobalColor.black)
        pen.setWidth(1)
        
        brush = QtGui.QBrush(Qt.GlobalColor.black, Qt.BrushStyle.SolidPattern)
        painter.setBrush(brush)
        
        painter.drawRect(0, 0, self.width(), self.height())
        
        if self._project is None:
            return super().paintEvent(event)
        
        
        painter.setPen(pen)
        for x in range(self._project.map.width):
            for y in range(self._project.map.height):
                tile_id = self._project.get_tile(x, y)
                if tile_id == 0:
                    continue  # empty tile
                
                text_rect = QtCore.QRect(x * self._tile_draw_size, 
                              y * self._tile_draw_size,
                              self._tile_draw_size,
                              self._tile_draw_size)
                
                painter.fillRect(text_rect, QtGui.QBrush(Qt.GlobalColor.red))
        
        
        pen.setColor(Qt.GlobalColor.darkGray)
        painter.setPen(pen)
        
        # vertical lines
        for gx in range(self._project.map.width + 1):
            px = gx * self._tile_draw_size
            painter.drawLine(px, 0, px, self._project.map.height * self._tile_draw_size)

        # horizontal lines
        for gy in range(self._project.map.height + 1):
            py = gy * self._tile_draw_size
            painter.drawLine(0, py, self._project.map.width * self._tile_draw_size, py)
        
        
        return super().paintEvent(event)
    
    def mousePressEvent(self, ev: QtGui.QMouseEvent) -> None:
        if ev.button() == Qt.MouseButton.LeftButton:
            # convert QPoint to my game point 
            pos = ev.position().toPoint()
            # emit the normalised (map index) position 
            
            # TODO hardcoded tile_id for now
            self.tile_drawn.emit(pos.x() // self._tile_draw_size, pos.y() // self._tile_draw_size, 1)  