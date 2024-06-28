from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton,  QPlainTextEdit,QMessageBox,QWidget

class Stats:
    def __init__(self):
        self.window = QMainWindow()
        self.window.resize(500, 400)
        self.window.move(300, 300)
        self.window.setWindowTitle('н��ͳ��')

        self.textEdit = QPlainTextEdit(self.window)
        self.textEdit.setPlaceholderText("������н�ʱ�")
        self.textEdit.move(10, 25)
        self.textEdit.resize(300, 350)

        self.button = QPushButton('ͳ��', self.window)
        self.button.move(380, 80)

        self.button.clicked.connect(self.handleCalc)

        self.button = QPushButton('ͳ��', self.window)
        self.button.move(380, 180)

        self.button.clicked.connect(self.handleCalc)


    def handleCalc(self):
        info = self.textEdit.toPlainText()

        # н��20000 ���� �� ���� ����Ա����
        salary_above_20k = ''
        salary_below_20k = ''
        for line in info.splitlines():
            if not line.strip():
                continue
            parts = line.split(' ')
            # ȥ���б��еĿ��ַ�������
            parts = [p for p in parts if p]
            name,salary,age = parts
            if int(salary) >= 20000:
                salary_above_20k += name + '\n'
            else:
                salary_below_20k += name + '\n'

        QMessageBox.about(self.window,
                    'ͳ�ƽ��',
                    f'''н��20000 ���ϵ��У�\n{salary_above_20k}
                    \nн��20000 ���µ��У�\n{salary_below_20k}'''
                    )

app = QApplication([])
stats = Stats()
stats.window.show()
app.exec_()

Stats.__init__()