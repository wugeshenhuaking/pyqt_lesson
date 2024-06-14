# import serial.tools.list_ports

# def find_and_print_serial_info():
#     ports = serial.tools.list_ports.comports()
#     for port in ports:
#         print(f"Port Name: {port.device}")
#         print(f"Description: {port.description}")
#         print(f"Hardware ID: {port.hwid}")
#         print("-------------------------")

# if __name__ == "__main__":
#     print("Available serial ports with detailed information:")
#     find_and_print_serial_info()

import sys
import serial.tools.list_ports
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QLabel, QComboBox
from PyQt5.QtCore import Qt

class SerialCommWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Serial Port Selector')

        # 创建中心窗口部件和布局
        central_widget = QWidget()
        layout = QVBoxLayout(central_widget)
        self.setCentralWidget(central_widget)
        
        # 添加标签说明
        label = QLabel("Available Serial Ports:", self)
        layout.addWidget(label)
        
        # 创建下拉列表框用于选择串口
        self.port_combo = QComboBox(self)
        layout.addWidget(self.port_combo)
        
        # 填充串口列表
        self.fill_serial_ports()
        
        # 设置布局
        self.setGeometry(100, 100, 400, 100)
    
    def fill_serial_ports(self):
        """填充串口下拉列表"""
        ports = serial.tools.list_ports.comports()
        for port, desc, hwid in sorted(ports):
            self.port_combo.addItem(desc)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SerialCommWindow()
    window.show()
    sys.exit(app.exec_())
