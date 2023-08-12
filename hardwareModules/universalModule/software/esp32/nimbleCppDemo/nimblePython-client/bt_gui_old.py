import string
import sys
from threading import Event
from gattlib import GATTRequester
from PyQt5 import QtWidgets, QtCore
import json

class SignalEmitter(QtCore.QObject):
    dataReceived = QtCore.pyqtSignal(str)

class Requester(GATTRequester):
    def __init__(self, wakeup, *args):
        GATTRequester.__init__(self, *args)
        self.wakeup = wakeup
        self.emitter = SignalEmitter()

    def on_notification(self, handle, data):
        processed_data = self.process_data(data)
        self.emitter.dataReceived.emit(processed_data)
        print(f"- handle notif:{processed_data}\n")
        self.wakeup.set()

    def on_indication(self, handle, data):
        processed_data = self.process_data(data)
        self.emitter.dataReceived.emit(processed_data)
        print(f"- handle indic:{processed_data}\n")
        self.wakeup.set()

    def process_data(self, data):
        clean_data = ''.join(filter(lambda x: x in string.printable, data.decode('utf-8')))
        try:
            parsed_data = json.loads(clean_data)
            return json.dumps(parsed_data, indent=4)
        except json.JSONDecodeError:
            return f"Invalid JSON data received: {clean_data}"

class ReceiveNotification(QtCore.QObject):
    def __init__(self, address):
        super(ReceiveNotification, self).__init__()

        self.received = Event()
        self.requester = Requester(self.received, address, False)

    def connect(self):
        self.requester.connect(True)

    def disconnect(self):
        self.requester.disconnect()

    def start_communication(self):
        self.requester.write_by_handle(0x0004, str(0x0200))
        self.received.wait()

    def stop_communication(self):
        self.requester.write_by_handle(0x0004, str(0x0000))

    def send_message(self, message):
        self.requester.write_by_handle(0x0003, str(message))

class MainApp(QtWidgets.QWidget):
    def __init__(self):
        super(MainApp, self).__init__()

        self.init_ui()

    def init_ui(self):
        self.setWindowTitle('BLE Communication')

        self.layout = QtWidgets.QVBoxLayout()

        self.input = QtWidgets.QLineEdit()
        self.input.setPlaceholderText("Enter IP Address...")
        self.input.setText("24:7F:0E:E6:AE:34")
        self.layout.addWidget(self.input)

        self.connect_button = QtWidgets.QPushButton("Connect")
        self.connect_button.clicked.connect(self.on_connect)
        self.layout.addWidget(self.connect_button)

        self.disconnect_button = QtWidgets.QPushButton("Disconnect")
        self.disconnect_button.setEnabled(False)
        self.disconnect_button.clicked.connect(self.on_disconnect)
        self.layout.addWidget(self.disconnect_button)

        self.start_button = QtWidgets.QPushButton("Start Communication")
        self.start_button.setEnabled(False)
        self.start_button.clicked.connect(self.on_start)
        self.layout.addWidget(self.start_button)

        self.stop_button = QtWidgets.QPushButton("Stop Communication")
        self.stop_button.setEnabled(False)
        self.stop_button.clicked.connect(self.on_stop)
        self.layout.addWidget(self.stop_button)

        self.message_input = QtWidgets.QLineEdit()
        self.message_input.setPlaceholderText("Enter Message...")
        self.layout.addWidget(self.message_input)

        self.send_button = QtWidgets.QPushButton("Send Message")
        self.send_button.clicked.connect(self.on_send)
        self.send_button.setEnabled(False)
        self.layout.addWidget(self.send_button)

        self.output = QtWidgets.QTextEdit()
        self.output.setReadOnly(True)
        self.layout.addWidget(self.output)

        self.setLayout(self.layout)

    def on_connect(self):
        self.notification = ReceiveNotification(self.input.text())
        self.notification.requester.emitter.dataReceived.connect(self.on_data_received)
        self.connect_button.setEnabled(False)
        self.disconnect_button.setEnabled(True)
        self.start_button.setEnabled(True)
        self.send_button.setEnabled(True)
        self.notification.connect()
        self.output.setText("")
        #self.on_start()

    def on_disconnect(self):
        self.notification.disconnect()
        self.connect_button.setEnabled(True)
        self.disconnect_button.setEnabled(False)
        self.start_button.setEnabled(False)
        self.stop_button.setEnabled(False)
        self.send_button.setEnabled(False)

    def on_start(self):
        self.notification.start_communication()
        self.start_button.setEnabled(False)
        self.stop_button.setEnabled(True)

    def on_stop(self):
        self.notification.stop_communication()
        self.stop_button.setEnabled(False)
        self.start_button.setEnabled(True)

    def on_send(self):
        self.notification.send_message(self.message_input.text())
        self.output.append(f"Sent: {self.message_input.text()}\n")
        self.message_input.clear()

    @QtCore.pyqtSlot(str)
    def on_data_received(self, data):
        self.output.append(data)

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    win = MainApp()
    win.show()
    sys.exit(app.exec_())
