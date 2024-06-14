from PyQt5.QtCore import QObject, pyqtSignal

class TextModel(QObject):
    textAppended = pyqtSignal(str)

    def __init__(self):
        super().__init__()
        self._texts = []

    def addText(self, text):
        self._texts.append(text)
        self.textAppended.emit(text)