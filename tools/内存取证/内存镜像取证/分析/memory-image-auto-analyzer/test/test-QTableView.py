import sys
from PySide6 import QtCore, QtGui, QtWidgets
from PySide6.QtCore import Qt
import pandas as pd


class TableModel(QtCore.QAbstractTableModel):

    def __init__(self, data):
        super(TableModel, self).__init__()
        self._data = data

    def data(self, index, role):
        if role == Qt.ItemDataRole.DisplayRole:
            value = self._data.iloc[index.row(), index.column()]
            return str(value)

    def rowCount(self, index):
        return self._data.shape[0]

    def columnCount(self, index):
        return self._data.shape[1]

    def headerData(self, section, orientation, role):
        # section is the index of the column/row.
        if role == Qt.ItemDataRole.DisplayRole:
            if orientation == Qt.Orientation.Horizontal:
                return str(self._data.columns[section])

            if orientation == Qt.Orientation.Vertical:
                return str(self._data.index[section])


class MainWindow(QtWidgets.QMainWindow):

    def __init__(self):
        super().__init__()

        self.table = QtWidgets.QTableView()

        data = pd.DataFrame([['SuggestedProfile(s)', 'Win7SP1x64,Win7SP0x64,Win2008R2SP0x64,Win2008R2SP1x64_24000,Win2008R2SP1x64_23418,Win2008R2SP1x64,Win7SP1x64_24000,Win7SP1x64_23418'], ['ASLayer1', 'WindowsAMD64PagedMemory(KernelAS)'], ['ASLayer2', 'FileAddressSpace(/home/randark/Snapshot6.vmem)'], ['PAEtype', 'NoPAE'], ['DTB', '0x187000L'], ['KDBG', '0xf80002c4a0a0L'], ['NumberofProcessors', '1'], ['ImageType(ServicePack)', '1'], ['KPCRforCPU0', '0xfffff80002c4bd00L'], ['KUSER_SHARED_DATA', '0xfffff78000000000L'], ['Imagedateandtime', '2020-12-2706', '20', '05UTC+0000'], ['Imagelocaldateandtime', '2020-12-2622', '20', '05-0800']], columns=['A', 'B', 'C', 'D'], index=['Row 1', 'Row 2', 'Row 3', 'Row 4', 'Row 5', 'Row 5', 'Row 5', 'Row 5', 'Row 5', 'Row 5', 'Row 5', 'Row 5'])

        self.model = TableModel(data)
        self.table.setModel(self.model)

        self.setCentralWidget(self.table)


app = QtWidgets.QApplication(sys.argv)
window = MainWindow()
window.show()
app.exec()