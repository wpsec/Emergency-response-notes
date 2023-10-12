import sys
from PySide6.QtWidgets import QApplication, QWidget, QTableView, QMainWindow, QVBoxLayout, QLineEdit, QHeaderView
from PySide6.QtCore import Qt, QSortFilterProxyModel, QAbstractTableModel,QRegularExpression


class TableModel(QAbstractTableModel):
    def __init__(self, data, headers):
        super().__init__()
        self._data = data
        self.headers = headers

    def data(self, index, role):
        if role == Qt.DisplayRole:
            return str(self._data[index.row()][index.column()])

    def rowCount(self, index):
        return len(self._data)

    def columnCount(self, index):
        return len(self._data[0])

    def headerData(self, section, orientation, role):
        if role == Qt.DisplayRole and orientation == Qt.Horizontal:
            return self.headers[section]


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.table = QTableView()

        data = [
            [4, 9, 2],
            [1, "hello", 0],
            [3, 5, 0],
            [3, 3, "what"],
            ["this", 8, 9],
        ]

        headers = ['Column 1', 'Column 2', 'Column 3']

        self.model = TableModel(data, headers)
        self.proxy_model = QSortFilterProxyModel()
        self.proxy_model.setFilterKeyColumn(-1)
        self.proxy_model.setSourceModel(self.model)

        self.proxy_model.sort(0, Qt.AscendingOrder)

        self.table.setModel(self.proxy_model)

        self.searchbar = QLineEdit()

        self.searchbar.textChanged.connect(self.filter_data)

        layout = QVBoxLayout()
        layout.addWidget(self.searchbar)
        layout.addWidget(self.table)

        container = QWidget()
        container.setLayout(layout)

        self.setCentralWidget(container)

        self.setup_table_header()

    def setup_table_header(self):
        header = self.table.horizontalHeader()
        header.setSectionResizeMode(QHeaderView.ResizeToContents)
        header.setStretchLastSection(True)

    def filter_data(self, text):
        regex = QRegularExpression(text)

        self.proxy_model.setFilterRegularExpression(regex)
        self.proxy_model.setFilterCaseSensitivity(Qt.CaseInsensitive)


app = QApplication(sys.argv)
window = MainWindow()
window.show()
sys.exit(app.exec())
