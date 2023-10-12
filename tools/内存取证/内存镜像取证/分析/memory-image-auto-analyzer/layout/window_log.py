import logging
from PySide6.QtWidgets import *


class QTextEditLogger(logging.Handler):

    def __init__(self, parent):
        super().__init__()
        self.widget = QPlainTextEdit(parent)
        self.widget.setReadOnly(True)
        self.widget.setLineWrapMode(QPlainTextEdit.LineWrapMode.NoWrap)
        self.widget.verticalScrollBar().minimum()

    def emit(self, record):
        msg = self.format(record)
        self.widget.appendPlainText(msg)

class LogWindow(QWidget):

    def __init__(self, parent=None):
        super().__init__(parent)

        self.setWindowTitle("日志输出")

        self.logTextBox = QTextEditLogger(self)
        self.logTextBox.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - module: %(module)s - funcName: %(funcName)s\n--> %(message)s\n'))
        logging.getLogger().addHandler(self.logTextBox)
        logging.getLogger().setLevel(logging.DEBUG)

        self.layout = QVBoxLayout()
        message = QLabel("如遇到非预期错误，请提供详细日志")
        self.layout.addWidget(message)
        self.layout.addWidget(self.logTextBox.widget)
        self.setLayout(self.layout)
        self.setMinimumSize(600, 400)

    def closeEvent(self, event):
        self.hide()