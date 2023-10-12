import sys
from PySide6.QtWidgets import *  #here I'm using PySide2, but you can use PyQt5 as well


class MainWindow(QMainWindow):

    def __init__(self, parent=None):
        super(MainWindow, self).__init__(parent)
        self.setGeometry(50, 50, 400, 450)

        ToolTab = UIToolTab(self)
        ToolTab.CPSBTN.clicked.connect(self.gotoP1)
        Window = UIWindow(self)
        Window.ToolsBTN.clicked.connect(self.gotoP2)
        self.stack = QStackedWidget(self)
        self.stack.addWidget(ToolTab)
        self.stack.addWidget(Window)
        self.setCentralWidget(self.stack)
        self.show()

    def gotoP2(self):
        self.setWindowTitle("Page1")
        self.stack.setCurrentIndex(0)

    def gotoP1(self):
        self.setWindowTitle("Page2")
        self.stack.setCurrentIndex(1)


class UIWindow(QWidget):

    def __init__(self, parent=None):
        super(UIWindow, self).__init__(parent)
        self.ToolsBTN = QPushButton('go P1', self)
        self.ToolsBTN.move(50, 350)


class UIToolTab(QWidget):

    def __init__(self, parent=None):
        super(UIToolTab, self).__init__(parent)
        self.CPSBTN = QPushButton("go P2", self)
        self.CPSBTN.move(100, 350)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MainWindow()
    sys.exit(app.exec_())