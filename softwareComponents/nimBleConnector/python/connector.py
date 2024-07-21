import asyncio
from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic

import protoc.rofi as rofi  # Import the generated betterproto module
from random import random
import logging
import coloredlogs
from random import random

logger = logging.getLogger(__name__)
coloredlogs.install(
    level="INFO", fmt="%(asctime)s %(levelname)s %(message)s", datefmt="%H:%M:%S"
)

# Define the UUIDs
CHARACT_UUID_ROFI_STATE = "D0F1"
CHARACT_UUID_ROFI_REQ_SPONSE = "D0F2"


# Function to decode SensorData from protobuf using betterproto
def decode_rofi_state(data) -> rofi.RofiState:
    # sensor_data = sensor.SensorData(bytes(data))
    rofi_state = rofi.RofiState().parse(bytes(data))
    logger.info(
        f"RoFI -> ID: {rofi_state.rofi_id}, PackedID: {rofi_state.packet_id}, Random: {rofi_state.random_number}"
    )
    return rofi_state


def encode_rofi_request(packed_id: int):
    rofi_request = rofi.RofiRequest(
        packet_id=packed_id,
        rofi_id=0,
        command=rofi.CommandTypeRequest.DEVICE,
        device=rofi.DeviceCommand(command=rofi.DeviceCommandType.SET_ID, set_id=33),
    )
    return bytes(rofi_request)


def decode_rofi_response(data, expected_packed_id: int) -> bool:
    rofi_response = rofi.RofiResponse().parse(bytes(data))
    logger.info(
        f"RoFI -> ID: {rofi_response.rofi_id}, PackedID: {rofi_response.packet_id}, IsSuccess: {rofi_response.success}, message: {rofi_response.message}"
    )
    assert rofi_response.packet_id == expected_packed_id
    return rofi_response.success


def rofi_state_notification_handler(
    characteristic: BleakGATTCharacteristic, data: bytearray
):
    """Simple notification handler which prints the data received."""
    logger.info("%s: %r", characteristic.description, data)
    rofi_state = decode_rofi_state(data)
    logger.info(f"RoFI -> ID: {rofi_state.rofi_id}, PackedID: {rofi_state.packet_id}")


class Connector:
    def __init__(self):
        self.devices = []
        self.counter = 0

    def _filter_devices(self, id: int | None = None):
        # Filter devices by name starting with "RoFI-" + optional ID
        if id is None:
            return [
                device
                for device in self.devices
                if device.name and device.name.startswith("RoFI-")
            ]
        else:
            return [
                device
                for device in self.devices
                if device.name and device.name.startswith(f"RoFI-{id}")
            ]

    async def scan(self, force_show_all: bool = False):
        """Scan for RoFI devices."""
        logger.info("Scanning for devices...")
        self.devices = await BleakScanner.discover()

        if not force_show_all:
            self.devices = self._filter_devices()

        return self.devices

    async def connect(self, id):
        """Connect to a RoFI device with a given ID."""
        self.devices = await self.scan()
        if id < 0 or id >= len(self.devices):
            logger.error(f"No device found with ID: {id}")
            return

        device = self.devices[id]
        logger.info(f"Connecting to {device.name} ({device.address})...")

        async with BleakClient(device) as client:
            logger.info(f"Connected to {device.name}")

            packed_id = 0

            try:
                await client.start_notify(
                    CHARACT_UUID_ROFI_STATE, rofi_state_notification_handler
                )  # stop_notify

                while True:
                    # # Read from the characteristic
                    # data = await client.read_gatt_char(CHARACT_UUID_ROFI_COMM)
                    # # logger.info(f"Received data: {data}")

                    # decode_rofi_state(data)

                    # Send a request to the device

                    await client.write_gatt_char(
                        CHARACT_UUID_ROFI_REQ_SPONSE, encode_rofi_request(packed_id)
                    )
                    logger.info(f"Sent request to {device.name}")

                    response = await client.read_gatt_char(CHARACT_UUID_ROFI_REQ_SPONSE)
                    logger.info(f"Received response to request: {response}")
                    decode_rofi_response(response, 0)
                    # decode_test(response, 0)
                    logger.info("Decoded response to request")

                    # packed_id += 2  # Increment the packed ID

                    await asyncio.sleep(5)  # Delay for 2 seconds
            except Exception as e:
                logger.error(f"Error: {e}")
