from PySide6.QtWidgets import *
import sys

class GroupBox(QWidget):

    def __init__(self):
        QWidget.__init__(self)

        self.setWindowTitle("GroupBox")
        layout = QGridLayout()
        self.setLayout(layout)

        groupbox = QGroupBox("GroupBox Example")
        groupbox.setCheckable(True)
        layout.addWidget(groupbox)
        
        vbox = QVBoxLayout()
        groupbox.setLayout(vbox)

        radiobutton = QRadioButton("RadioButton 1")
        vbox.addWidget(radiobutton)
        
        radiobutton = QRadioButton("RadioButton 2")
        vbox.addWidget(radiobutton)

        radiobutton = QRadioButton("RadioButton 3")
        vbox.addWidget(radiobutton)

        radiobutton = QRadioButton("RadioButton 4")
        vbox.addWidget(radiobutton)
        
        
app = QApplication(sys.argv)
screen = GroupBox()
screen.show()
sys.exit(app.exec_())