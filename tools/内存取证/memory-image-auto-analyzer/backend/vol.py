from PySide6.QtCore import QProcess
import logging
from backend.res import core_res


class vol_backend_v2(QProcess):
    # FIXME 后端改写，换为QThreadPool和QRunnable

    def __init__(self, imagefile: str, module: str, func_finished, profile=None) -> None:
        super().__init__()
        self.imagefile = imagefile
        self.module = module
        self.func_finished = func_finished
        self.profile = profile

    def run(self):
        """
        调用Volatility2进行镜像分析
        """
        self.process = QProcess()  # Keep a reference to the QProcess (e.g. on self) while it's running.
        self.process.readyReadStandardOutput.connect(self.handle_stdout)
        self.process.readyReadStandardError.connect(self.handle_stderr)
        self.process.stateChanged.connect(self.handle_state)
        # self.process.finished.connect(self.process_finished)
        self.process.finished.connect(self.func_finished)
        if self.profile != None:
            print("vol.py", " ".join(["-f", self.imagefile, "--profile="+self.profile, self.module]))
            self.process.start("vol.py", ["-f", self.imagefile, "--profile="+self.profile, self.module])
        else:
            print("vol.py", " ".join(["-f", self.imagefile, self.module]))
            self.process.start("vol.py", ["-f", self.imagefile, self.module])

    def handle_stderr(self):
        data = self.process.readAllStandardError()
        stderr = bytes(data).decode()
        logging.error(stderr)

    def handle_stdout(self):
        data = self.process.readAllStandardOutput()
        stdout = bytes(data).decode()
        logging.info(stdout)
        core_res.add_res(self.module, stdout)

    def handle_state(self, state):
        states = {
            QProcess.NotRunning: 'Not running',
            QProcess.Starting: 'Starting',
            QProcess.Running: 'Running',
        }
        state_name = states[state]
        logging.debug(f"State changed: {state_name}")
