import logging
from pathlib import Path
from typing import Dict, Any

from PySide6 import QtCore, QtGui
from PySide6.QtCore import Qt, Signal

from PySide6.QtWidgets import (QLabel, QCheckBox, QPushButton, QSlider,
                               QVBoxLayout, QHBoxLayout, QWidget, QDialog,
                               QDialogButtonBox, QInputDialog, QScrollArea,
                               QButtonGroup)

from project import Project # only for type hinting, no direct usage
from texture_manager import TextureManager

class TileListManagerWidget(QWidget):
    tile_selected = Signal(int)  # tile_id
    
    def __init__(self, texture_manager: TextureManager, thumb_size: int = 24):
        super().__init__()
        self._thumb_size = thumb_size

        layout = QVBoxLayout(self)
        layout.setContentsMargins(6, 6, 6, 6)
        layout.setSpacing(6)
        layout.addWidget(QLabel("Texture List"))

        self.texture_manager = texture_manager

        # Scrollable container for the list
        self._scroll = QScrollArea(self)
        self._scroll.setWidgetResizable(True)
        self._container = QWidget()
        self._vbox = QVBoxLayout(self._container)
        self._vbox.setContentsMargins(0, 0, 0, 0)
        self._vbox.setSpacing(4)
        self._scroll.setWidget(self._container)
        layout.addWidget(self._scroll)

        # map of id -> button
        self._buttons: dict[int, QPushButton] = {}

        self._build_list(self.texture_manager.get_textures())

    def _build_list(self, textures: Dict[int, Dict[str, Any]]):
        # Clear current items
        for i in reversed(range(self._vbox.count())):
            item = self._vbox.itemAt(i)
            w = item.widget() if item else None
            if w:
                w.setParent(None)

        self._buttons.clear()
        # Recreate button group (safe reset)
        self._btn_group = QButtonGroup(self)
        self._btn_group.setExclusive(True)
        # custom property "tile_id" used to store tile id
        self._btn_group.buttonClicked.connect(lambda btn: self._on_button_clicked(btn.property("tile_id")))

        for tex_id, info in textures.items():
            name: str = info.get("name", f"tex_{tex_id}")
            # remove "tex_" prefix for display
            name = name.removeprefix("tex_")
            
            color = info.get("rcolor", TextureManager.INVALID_TEX_COLOR)

            # create small colored pixmap for the left box
            pix = QtGui.QPixmap(self._thumb_size, self._thumb_size)
            pix.fill(QtGui.QColor(color))

            btn = QPushButton(name)
            btn.setCheckable(True)
            btn.setIcon(QtGui.QIcon(pix))
            btn.setIconSize(QtCore.QSize(self._thumb_size, self._thumb_size))
            # left-align text and give padding; checked state shows highlight
            btn.setStyleSheet(
                "QPushButton { text-align: left; padding: 6px; border: 1px solid transparent; }"
                "QPushButton:checked { border: 2px solid #00aaff; background-color: #B1B1B1; }"
            )
            
            # store tile id in property for retrieval on click
            btn.setProperty("tile_id", tex_id)
            
            self._btn_group.addButton(btn)
            self._buttons[tex_id] = btn
            self._vbox.addWidget(btn)

        self._vbox.addStretch()

    def _on_button_clicked(self, tex_id: int):
        # Emit selected tile id
        self.tile_selected.emit(tex_id)
        logging.info(f"Selected tile ID: {tex_id}")

    def select_tile(self, tex_id: int):
        btn = self._buttons.get(tex_id)
        if btn:
            btn.setChecked(True)

class MapCanvasWidget(QWidget):
    tile_drawn = Signal(int, int, int)  # x, y, tile_id
    
    def __init__(self, texture_manager: TextureManager, tile_draw_size: int = 16):
        super().__init__()
        self.setFixedSize(512, 512)
        
        self._tile_draw_size = tile_draw_size
        self._texture_manager = texture_manager
        
        self._project: Project | None = None
        
        self._selected_tile_id: int = 0
        
        # state for drag/continuous paint
        self._mouse_down = False
        self._last_emitted_tile: tuple[int, int] = (-1, -1)
        self._last_emitted_tile_id: int = -1
    
    @property
    def selected_tile_id(self) -> int:
        return self._selected_tile_id
    
    @selected_tile_id.setter
    def selected_tile_id(self, value: int):
        if value < 0:
            logging.warning(f"Tried to set invalid tile ID: {value}")
            return
        
        self._selected_tile_id = value
        
    @QtCore.Slot(int)
    def set_selected_tile_id(self, value: int):
        """
        Signal compatible wrapper
        """
        self.selected_tile_id = value
    
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
        
        # to get the color of the texture to draw
        textures = self._texture_manager.get_textures()
        
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
                
                color = textures[tile_id]["rcolor"]
                painter.fillRect(text_rect, QtGui.QBrush(QtGui.QColor(color)))
        
        
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
        pos = ev.position().toPoint() if hasattr(ev, "position") else ev.pos()

        if ev.button() == Qt.MouseButton.LeftButton:
            self._mouse_down = True
            tx = pos.x() // self._tile_draw_size
            ty = pos.y() // self._tile_draw_size
            self._emit_tile_if_changed(tx, ty, self._selected_tile_id)
        elif ev.button() == Qt.MouseButton.RightButton:
            self._mouse_down = True
            tx = pos.x() // self._tile_draw_size
            ty = pos.y() // self._tile_draw_size
            self._emit_tile_if_changed(tx, ty, 0)

        return super().mousePressEvent(ev)

    def mouseMoveEvent(self, ev: QtGui.QMouseEvent) -> None:
        # Only act while mouse button is held (dragging); normal move events otherwise are ignored
        if not self._mouse_down:
            return super().mouseMoveEvent(ev)

        pos = ev.position().toPoint()
        tx = pos.x() // self._tile_draw_size
        ty = pos.y() // self._tile_draw_size

        # decide paint vs erase based on which button is down
        buttons = ev.buttons()
        if buttons & Qt.MouseButton.LeftButton:
            self._emit_tile_if_changed(tx, ty, self._selected_tile_id)
        elif buttons & Qt.MouseButton.RightButton:
            self._emit_tile_if_changed(tx, ty, 0)

        return super().mouseMoveEvent(ev)

    def mouseReleaseEvent(self, ev: QtGui.QMouseEvent) -> None:
        # stop drag/continuous mode and reset last-emitted state
        self._mouse_down = False
        self._last_emitted_tile = (-1, -1)
        self._last_emitted_tile_id = -1
        return super().mouseReleaseEvent(ev)

    def _emit_tile_if_changed(self, tx: int, ty: int, tile_id: int) -> None:
        # bounds & dedupe checks to prevent spamming identical emits
        if self._project is None or self._project.map is None:
            return
        if tx < 0 or ty < 0 or tx >= self._project.map.width or ty >= self._project.map.height:
            return
        if (tx, ty) == self._last_emitted_tile and tile_id == self._last_emitted_tile_id:
            return
        
        self.tile_drawn.emit(tx, ty, tile_id)
        self._last_emitted_tile = (tx, ty)
        self._last_emitted_tile_id = tile_id