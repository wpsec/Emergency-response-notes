import sys
from PySide6.QtCore import Qt, QRunnable, QThreadPool, Slot,Signal,QObject
from PySide6.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton, QTextEdit
from time import sleep

class CommandTask(QRunnable):
    def __init__(self, command):
        super().__init__()
        self.command = command

    @Slot()
    def run(self):
        # 执行需要在后台线程中完成的任务
        print("Task started")
        import subprocess
        result = subprocess.run(self.command, capture_output=True, text=True)
        output = result.stdout.strip()
        # sleep(3)
        print(f"Command: {self.command}, Output: {output}")
        self.signal.finished.emit(output)

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("Thread Example")
        self.layout = QVBoxLayout()
        self.button = QPushButton("Start Threads")
        self.button.clicked.connect(self.startThreads)
        self.layout.addWidget(self.button)
        self.console = QTextEdit(self)
        self.console.setReadOnly(True)
        self.layout.addWidget(self.console)
        self.setLayout(self.layout)

    def startThreads(self):
        threadpool = QThreadPool()
        threadpool.setMaxThreadCount(3)  # 设置最大线程数为 3

        self.button.setEnabled(False)
        self.console.clear()

        commands = ["date --help","whoami --help"]
        for command in commands:
            task = CommandTask(command.split(" "))
            task.signal = TaskSignal()
            task.signal.finished.connect(self.outputResult)
            threadpool.start(task)

    @Slot(str)
    def outputResult(self, result):
        self.console.append(result)

class TaskSignal(QObject):
    finished = Signal(str)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
