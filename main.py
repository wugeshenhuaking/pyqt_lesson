import model
import view
from PyQt5.QtWidgets import QApplication
import sys


if __name__ == "__main__":
    app = QApplication(sys.argv)

    model = model.TextModel()
    view = view.TextView(model)

    view.show()

    sys.exit(app.exec_())