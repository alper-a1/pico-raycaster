from pathlib import Path

from PySide6.QtCore import Qt, Signal
from PySide6.QtGui import QMouseEvent
from PySide6.QtWidgets import (
    QMainWindow,
    QWidget,
    QLabel,
    QVBoxLayout, QHBoxLayout,
    QStatusBar,
    QSpinBox,
    QCheckBox, 
    QPushButton,
    QMessageBox
)

from ui.custom_widgets import MapCanvasWidget, TileListManagerWidget
from project import Project
from texture_manager import TextureManager
class MapEditorMainWindow(QMainWindow):
    # map_size_changed = Signal(int, int) # width, height
    
    def __init__(self):
        super().__init__()
        self.project_root = Path(__file__).resolve().parents[3] # pico-raycaster root
        self.setWindowTitle("pico-raycaster — Map Editor")
        self.resize(1000, 700)

        # Project handle
        assets_path = Path.joinpath(self.project_root, "assets")
        self.project = Project(assets_path)

        # Texture manager instance
        texture_manager = TextureManager(assets_path)

        self.map_canvas = MapCanvasWidget(texture_manager)
        self.map_canvas.set_project(self.project)
        
        # min / max map size
        map_h_sb = QSpinBox()
        map_h_sb.setRange(16, 255)
        map_w_sb = QSpinBox()
        map_w_sb.setRange(16, 255)
        
        # initialize with default map size
        self.project.new_map(map_w_sb.value(), map_h_sb.value())
        
        # texture manager
        self.tile_list_widget = TileListManagerWidget(texture_manager)
        
        save_map_btn = QPushButton("Save Map")
        
        
        # =====================
        # Signals
        # =====================
        
        save_map_btn.clicked.connect(self.project.save_map)
        
        self.map_canvas.tile_drawn.connect(self.project.set_tile)
        self.project.map_changed.connect(self.map_canvas.repaint)
        
        # connect signals for map size changing (DOES NOT PRESERVE TILES)
        map_w_sb.valueChanged.connect(lambda val: self.project.new_map(val, map_h_sb.value()))
        map_h_sb.valueChanged.connect(lambda val: self.project.new_map(map_w_sb.value(), val))
        
        self.tile_list_widget.tile_selected.connect(self.map_canvas.set_selected_tile_id)
        
        # =====================
        # Layout
        # =====================

        central = QWidget()
        h_layout = QHBoxLayout(central)
        h_layout.setContentsMargins(6, 6, 6, 6)
        h_layout.setSpacing(8)

        # Left: canvas area (takes 2/3 of space)
        canvas_container = QWidget()
        canvas_layout = QVBoxLayout(canvas_container)
        canvas_layout.setContentsMargins(0, 0, 0, 0)
        canvas_layout.addWidget(self.map_canvas)
        h_layout.addWidget(canvas_container, stretch=2)

        # Right: controls sidebar (vertical list)
        sidebar = QWidget()
        sidebar_layout = QVBoxLayout(sidebar)
        sidebar_layout.setContentsMargins(6, 6, 6, 6)
        sidebar_layout.setSpacing(8)

        title = QLabel("Map Editor — controls")
        title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        sidebar_layout.addWidget(title)

        sidebar_layout.addWidget(QLabel("Left Click: Draw Tile"))
        sidebar_layout.addWidget(QLabel("Right Click: Delete Tile"))
        
        sidebar_layout.addWidget(QLabel("Map Width"))
        sidebar_layout.addWidget(map_w_sb)

        sidebar_layout.addWidget(QLabel("Map Height"))
        sidebar_layout.addWidget(map_h_sb)
        
        sidebar_layout.addSpacing(10)
        
        sidebar_layout.addWidget(self.tile_list_widget)

        # add other controls here (tool selection, save/load buttons, etc.)
        sidebar_layout.addStretch()  # push controls to top

        h_layout.addWidget(sidebar, stretch=1)

        self.setCentralWidget(central)
        # Status bar
        self.setStatusBar(QStatusBar(self))
        self.statusBar().showMessage(f"Project: {self.project_root}")

    def closeEvent(self, event):
        if not self.project.dirty:
            event.accept()
            return
        
        # prompt to save changes
        reply = QMessageBox.question(self, "Unsaved Changes", 
                                     "You have unsaved changes. Do you want to save before exiting?",
                                     buttons=QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No | QMessageBox.StandardButton.Cancel,
                                     defaultButton=QMessageBox.StandardButton.Yes)

        if reply == QMessageBox.StandardButton.Yes:
            self.project.save_map("mapdata.xip")  # TODO: hardcoded name -- see project.py comments
            event.accept()
        elif reply == QMessageBox.StandardButton.Cancel:
            event.ignore()
            