import asyncio
import sys

from PyQt6.QtCore import Qt
from PyQt6.QtWidgets import (
    QApplication,
    QMainWindow,
    QPushButton,
    QLabel,
    QVBoxLayout,
    QWidget,
    QLineEdit,
    QSlider,
)

from softwareComponents.nimBleConnector.python.rofi_connector import RofiConnector
from softwareComponents.nimBleConnector.python.rofi_angle import Angle
from qasync import QEventLoop, asyncSlot, QtGui
from rofi_device import RofiDevice


class RofiGUI(QMainWindow):
    def __init__(self):
        super().__init__()

        self.status_label: QLabel | None = None
        self.packet_status_label: QLabel | None = None
        self.input_number: QLineEdit | None = None
        self.connect_button: QPushButton | None = None
        self.disconnect_button: QPushButton | None = None
        self.slider: QSlider | None = None
        self.notification_label: QLabel | None = None
        self.slider_label: QLabel | None = None
        self.slider_response: QSlider | None = None

        self.connector = RofiConnector(try_reconnect=True, wait_for_response=True)
        self.rofi_device: RofiDevice | None = None
        self.rofi_device_id = 0
        self.initUI()

    def initUI(self):
        self.setWindowTitle("RoFI BLE Control App - minimal")

        # print connected or disconnected - left corner
        self.status_label = QLabel("Status: Disconnected", self)
        self.status_label.setAlignment(Qt.AlignmentFlag.AlignLeft)

        self.packet_status_label = QLabel("", self)
        self.packet_status_label.setAlignment(Qt.AlignmentFlag.AlignRight)

        self.input_number = QLineEdit(self)
        self.input_number.setPlaceholderText("RoFI device number")
        self.input_number.setText(str(self.rofi_device_id))
        self.input_number.setValidator(QtGui.QIntValidator())
        self.input_number.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.input_number.textChanged.connect(self.edit_rofi_device_id)

        self.connect_button = QPushButton(
            f"Connect to RoFI ID: {self.rofi_device_id}", self
        )
        self.connect_button.clicked.connect(self.connect_rofi_device)

        self.disconnect_button = QPushButton("Disconnect RoFI Device", self)
        self.disconnect_button.clicked.connect(self.disconnect_rofi_device)

        # add space between buttons
        self.space = QLabel("", self)

        self.move_button = QPushButton("Move Servo 0", self)
        self.move_button.clicked.connect(self.rofi_move_servo)

        # Slider info
        self.slider_label = QLabel("Servo 0 position", self)

        self.slider = QSlider(Qt.Orientation.Horizontal, self)
        self.slider.setRange(-40, 40)
        # self.slider.valueChanged.connect(self.rofi_move_servo_slider)
        self.slider.sliderReleased.connect(self.rofi_move_servo_slider)
        self.slider.setValue(0)
        self.slider.setTickInterval(1)

        # Slider showing response - disabled
        self.slider_response = QSlider(Qt.Orientation.Horizontal, self)
        self.slider_response.setRange(-40, 40)
        self.slider_response.setValue(0)
        self.slider_response.setEnabled(False)

        # Label to display notification data
        self.notification_label = QLabel("Notifications will appear here", self)

        # scrolling box where will be added all the console output
        self.console_output_box = QVBoxLayout()
        self.console_output = QLabel("Console output:", self)
        self.console_output_box.addWidget(self.console_output)
        self.console_output_box.setAlignment(Qt.AlignmentFlag.AlignTop)
        self.console_output.setAlignment(Qt.AlignmentFlag.AlignTop)
        # self.console_output.setWordWrap(True)

        # Layout
        layout = QVBoxLayout()
        layout.addWidget(self.status_label)
        layout.addWidget(self.packet_status_label)
        layout.addWidget(self.input_number)
        layout.addWidget(self.connect_button)
        layout.addWidget(self.disconnect_button)
        layout.addWidget(self.space)
        layout.addWidget(self.move_button)
        layout.addWidget(self.slider_label)
        layout.addWidget(self.slider)
        layout.addWidget(self.slider_response)

        layout.addWidget(self.notification_label)
        layout.addLayout(self.console_output_box)

        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

    def show_notification(self, message):
        self.notification_label.setText(message)
        print(message)

    def edit_rofi_device_id(self, rofi_id):
        if rofi_id == "":
            rofi_id = 0
        self.rofi_device_id = rofi_id
        self.connect_button.setText(f"Connect to RoFI ID: {self.rofi_device_id}")
        self.show_notification(f"RoFI device ID set to: {self.rofi_device_id}")

    def on_rofi_state_update(self, rofi: RofiDevice, packet_id: int):
        self.packet_status_label.setText(f"Packet ID: {packet_id}")

        slider_rad = rofi.get_joint(0).get_position()
        slider_deg = int(Angle.from_rad(slider_rad).to_deg())
        self.slider_response.setValue(slider_deg)
        self.show_notification(
            f"RoFI state updated: Servo 0 position: {slider_rad} degrees"
        )

    def on_rofi_state_update_error(
        self, rofi: RofiDevice, packet_id: int, error_message: str
    ):
        self.packet_status_label.setText(f"Packet ID ERR: {packet_id}")
        self.console_output.setText(f"Error: {error_message}")

    @asyncSlot()
    async def connect_rofi_device(self):
        self.show_notification("Connecting to RoFI device")
        self.status_label.setText("Status: Connecting...")
        try:
            self.rofi_device = await self.connector.connect(rofi_id=self.rofi_device_id)
            self.rofi_device.set_on_update_rofi_state_callback(
                self.on_rofi_state_update
            )
            self.rofi_device.set_on_update_rofi_state_error_callback(
                self.on_rofi_state_update_error
            )
            self.rofi_device.set_on_disconnect_callback(
                lambda: self.status_label.setText("Status: Disconnected")
            )
            self.status_label.setText("Status: Connected")
            self.show_notification("RoFI device found and connected")
        except Exception as e:
            self.show_notification(f"Error connecting to BLE device: {e}")

    @asyncSlot()
    async def disconnect_rofi_device(self):
        if not self.rofi_device:
            self.show_notification("RoFI device not connected")
            return

        try:
            await self.rofi_device.disconnect()
            self.status_label.setText("Status: Disconnected")
            self.show_notification("RoFI device disconnected")
        except Exception as e:
            self.show_notification(f"Error disconnecting from BLE device: {e}")

    @asyncSlot()
    async def rofi_move_servo(self):
        if not self.rofi_device:
            self.show_notification("RoFI device not connected")
            return

        joint_0 = self.rofi_device.get_joint(0)
        self.show_notification("Setting position to 45 degrees")
        await joint_0.set_position(Angle.from_deg(45).to_rad(), 1)
        await asyncio.sleep(0.1)

        self.show_notification("Setting position to 20 degrees")
        await joint_0.set_position(Angle.from_deg(20).to_rad(), 1)
        await asyncio.sleep(0.1)

    @asyncSlot()
    async def rofi_move_servo_slider(self):
        if not self.rofi_device:
            self.show_notification("RoFI device not connected")
            return

        value = self.slider.value()
        joint_0 = self.rofi_device.get_joint(0)
        self.show_notification(f"Setting position to {value} degrees")
        await joint_0.set_position(Angle.from_deg(value).to_rad(), 0.5)


if __name__ == "__main__":
    app = QApplication(sys.argv)

    event_loop = QEventLoop(app)
    asyncio.set_event_loop(event_loop)

    ex = RofiGUI()
    ex.show()

    with event_loop:
        event_loop.run_forever()
