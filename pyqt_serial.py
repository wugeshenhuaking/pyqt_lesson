import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QPushButton, QTextEdit, QLabel, QComboBox
from PyQt5.QtCore import Qt

class SerialCommWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Serial Communication')
        
        # 创建中心窗口部件和布局
        central_widget = QWidget()
        layout = QVBoxLayout(central_widget)
        self.setCentralWidget(central_widget)
        
        # 串口选择下拉菜单
        self.port_combo = QComboBox()
        layout.addWidget(QLabel('Select Port:'))
        layout.addWidget(self.port_combo)
        
        # 打开/关闭串口按钮
        self.open_close_button = QPushButton('Open Serial Port')
        layout.addWidget(self.open_close_button)
        
        # 发送数据输入框
        self.send_text = QTextEdit()
        layout.addWidget(self.send_text)
        
        # 接收数据显示区域
        self.receive_text = QTextEdit(readOnly=True)
        layout.addWidget(self.receive_text)
        
        # 设置布局
        self.setGeometry(100, 100, 600, 400)

### 步骤 3: 实现串口通信逻辑


# ... 上述代码之后继续

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SerialCommWindow()
    window.show()
    sys.exit(app.exec_())