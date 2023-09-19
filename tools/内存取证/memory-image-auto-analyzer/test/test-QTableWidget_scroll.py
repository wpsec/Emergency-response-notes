import sys
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

class Window(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)

        self.setWindowTitle("Scrolling QTableWidget smoothly BY MOUSE WHEEL")
        
        label = QLabel("singleStep:")
        self.spinbox = QSpinBox()
        self.spinbox.setValue(1)
        self.spinbox.setMinimum(1)
        self.spinbox.setMaximum(200)
        self.spinbox.valueChanged.connect(self.on_value_changed)

        self.widget = QTableWidget(100, 5)

        for i in range(100):
            for j in range(5):
                self.widget.setItem(i, j, QTableWidgetItem(str(i+j)))

        self.widget.setVerticalScrollMode(QAbstractItemView.ScrollPerPixel)
        self.widget.setHorizontalScrollMode(QAbstractItemView.ScrollPerPixel)
        #self.widget.verticalScrollBar().setSingleStep(1)
        self.set_single_step()

        spinbox_layout = QHBoxLayout()
        spinbox_layout.addStretch()
        spinbox_layout.addWidget(label)
        spinbox_layout.addWidget(self.spinbox)

        layout = QVBoxLayout()
        layout.addLayout(spinbox_layout)
        layout.addWidget(self.widget)
        self.setLayout(layout)

    def on_value_changed(self, step):
        self.set_single_step()

    def set_single_step(self):
        self.widget.verticalScrollBar().setSingleStep(5)
        self.widget.horizontalScrollBar().setSingleStep(5)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = Window()
    window.resize(800, 600)
    window.show()
    sys.exit(app.exec())