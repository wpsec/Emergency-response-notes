from PySide6.QtWidgets import QTableWidget, QLineEdit, QPushButton, QApplication, QMainWindow, QVBoxLayout, QWidget, QTableWidgetItem
from PySide6.QtCore import Qt

import random, string, sys


class MainWindow(QMainWindow):

    def __init__(self):
        super().__init__()


        self.query = QLineEdit()
        self.query.setPlaceholderText("Search...")
        self.query.textChanged.connect(self.search)

        n_rows = 50
        n_cols = 4

        self.table = QTableWidget()
        self.table.setRowCount(n_rows)
        self.table.setColumnCount(n_cols)

        for c in range(0, n_cols):
            for r in range(0, n_rows):
                s = ''.join(random.choice(string.ascii_lowercase) for n in range(10))
                i = QTableWidgetItem(s)
                self.table.setItem(c, r, i)

        layout = QVBoxLayout()

        layout.addWidget(self.query)
        layout.addWidget(self.table)

        w = QWidget()
        w.setLayout(layout)
        self.setCentralWidget(w)

    def search(self, s):
        # clear current selection.
        self.table.setCurrentItem(None)

        if not s:
            # Empty string, don't search.
            return

        matching_items = self.table.findItems(s, Qt.MatchContains)
        if matching_items:
            # We have found something.
            for item in matching_items:
                item.setSelected(True)


app = QApplication(sys.argv)
w = MainWindow()
w.show()

app.exec()