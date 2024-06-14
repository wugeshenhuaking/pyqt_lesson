from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLineEdit, QPushButton, QListWidget

class TextView(QWidget):
    def __init__(self, model):
        super().__init__()
        self.model = model

        layout = QVBoxLayout()

        self.lineEdit = QLineEdit()
        self.addButton = QPushButton("Add")
        self.listWidget = QListWidget()

        layout.addWidget(self.lineEdit)
        layout.addWidget(self.addButton)
        layout.addWidget(self.listWidget)

        self.addButton.clicked.connect(self.onAddButtonClicked)
        self.model.textAppended.connect(self.listWidget.addItem)

        self.setLayout(layout)

    def onAddButtonClicked(self):
        text = self.lineEdit.text().strip()
        if text:
            self.model.addText(text)
            self.lineEdit.clear()