


import string
import sys
from threading import Event
from gattlib import GATTRequester
from PyQt5 import QtWidgets, QtCore, uic
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
        return clean_data
        # try:
        #     json_data = json.loads(clean_data)
        #     print(f"JSON data received: {json_data}")
        #     return json_data
        # except json.JSONDecodeError:
        #     return f"Invalid JSON data received: {clean_data}"





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

    def __init__(self):
        super(MainApp, self).__init__()

        uic.loadUi('/media/psf/Home/Documents/git/paradise-fi/RoFI/hardwareModules/universalModule/software/esp32/nimbleCppDemo/nimblePython-client/bt_gui.ui', self)
        self.input.setText("24:7F:0E:E6:AE:34")

        # Connect signals and slots
        self.connect_button.clicked.connect(self.on_connect)
        self.disconnect_button.clicked.connect(self.on_disconnect)
        self.start_button.clicked.connect(self.on_start)
        self.stop_button.clicked.connect(self.on_stop)
        self.send_button.clicked.connect(self.on_send)

    def on_connect(self):
        self.notification = ReceiveNotification(self.input.text())
        self.notification.requester.emitter.dataReceived.connect(self.on_data_received)
        self.connect_button.setEnabled(False)
        self.disconnect_button.setEnabled(True)
        self.start_button.setEnabled(True)
        self.send_button.setEnabled(True)
        self.notification.connect()
        self.output.setText("")

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
        self.update_ui(data)

    def update_ui(self, data):
        try:
            json_data = json.loads(data)
            print(f"JSON data received: {json_data}")
            self.slider0.setValue(int(json_data["joints"]["joint0"]["position"]))
            self.sliderText0.append(str(json_data["joints"]["joint0"]))
        except json.JSONDecodeError:
            return f"Invalid JSON data received: {data}"


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    win = MainApp()
    win.show()
    sys.exit(app.exec_())


		# {
		# 	"ID": 0,
		# 	"joints": {
		# 		"joint0": {
		# 			"velocity": null,
		# 			"position": null,
		# 			"torque": null,
		# 			"capabilities": {
		# 				"position": {
		# 					"max": null,
		# 					"min": null
		# 				},
		# 				"speed": {
		# 					"max": null,
		# 					"min": null
		# 				},
		# 				"torque": {
		# 					"max": null
		# 				}
		# 			}
		# 		}
		# 	},
		# 	"connectors": {
		# 		"connector1": {
		# 			"position": null
		# 		}
		# 	}
		# }