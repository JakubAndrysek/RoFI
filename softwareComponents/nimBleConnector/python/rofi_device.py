import asyncio
import logging
from typing import Callable

import coloredlogs
from bleak import BleakClient
from bleak.backends.characteristic import BleakGATTCharacteristic
from bleak.backends.device import BLEDevice

import protoc.rofi as rofi_pb  # Import the generated betterproto module

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
coloredlogs.install(
    level="INFO", fmt="%(asctime)s %(levelname)s %(message)s", datefmt="%H:%M:%S"
)

# Define the UUIDs
CHARACT_UUID_ROFI_STATE = "D0F1"
CHARACT_UUID_ROFI_REQ_SPONSE = "D0F2"


class RofiException(Exception):
    pass


# rofi exception if lib does not have received any data
class RofiNoDataException(RofiException):
    pass


class RofiCmd:
    def __init__(self, rofi: "Rofi", command_type: rofi_pb.CommandTypeRequest):
        self.rofi = rofi
        self.command_type = command_type

    def _test_connection(self):
        if self.rofi.client.is_connected:
            return
        if not self.rofi.try_reconnect:
            raise RofiException("RoFI device is not connected.")

        logger.warning("RoFI device is not connected. Reconnecting...")
        self.rofi.reconnect()

    async def write_command(self, rofi_request: rofi_pb.RofiRequest):
        self._test_connection()
        rofi_request.packet_id = self.rofi.packet_id
        self.rofi.packet_id += 1  # Increment the packet ID
        rofi_request.command = self.command_type

        lock = asyncio.Lock()
        async with lock:
            await self.rofi.client.write_gatt_char(
                CHARACT_UUID_ROFI_REQ_SPONSE, rofi_request.SerializeToString()
            )

        if not self.rofi.wait_for_response:
            return

        lock = asyncio.Lock()
        async with lock:
            response_raw = await self.rofi.client.read_gatt_char(CHARACT_UUID_ROFI_REQ_SPONSE)
        response = rofi_pb.RofiResponse().parse(response_raw)
        if not response.success:
            raise RofiException(f"Error: {response.message}")
        if rofi_request.packet_id != response.packet_id - 1:  # response.packet_id is the next packet ID
            raise RofiException(
                f"Packet ID mismatch. Sent: {rofi_request.packet_id}, Expected: {rofi_request.packet_id + 1}, Received: {response.packet_id}"
            )


class RofiMessage(RofiCmd):
    def __init__(self, rofi: "Rofi"):
        super().__init__(rofi, rofi_pb.CommandTypeRequest.MESSAGE)

    async def send_message(self, message: str):
        message_command = rofi_pb.Message(message=message)
        await self.write_command(rofi_pb.RofiRequest(message=message_command))


class RofiDevice(RofiCmd):
    def __init__(self, rofi: "Rofi"):
        super().__init__(rofi, rofi_pb.CommandTypeRequest.DEVICE)

    async def set_id(self, rofi_id: int):
        device_command = rofi_pb.DeviceCommand(
            command=rofi_pb.DeviceCommandType.SET_ID, set_id=rofi_id
        )
        await self.write_command(rofi_pb.RofiRequest(device=device_command))

    async def reboot(self):
        device_command = rofi_pb.DeviceCommand(command=rofi_pb.DeviceCommandType.REBOOT)
        await self.write_command(rofi_pb.RofiRequest(device=device_command))


class RofiJoint(RofiCmd):
    def __init__(self, rofi: "Rofi", joint_id: int):
        super().__init__(rofi, rofi_pb.CommandTypeRequest.JOINT)
        self.joint_id = joint_id
        self.joint_state = rofi_pb.Joint()

    def get_velocity(self):
        return self.joint_state.velocity

    async def set_velocity(self, velocity: float):
        joint_command = rofi_pb.JointCommand(
            joint_id=self.joint_id,
            command=rofi_pb.JointCommandType.SET_JOINT_VELOCITY,
            set_velocity=velocity
        )
        await self.write_command(rofi_pb.RofiRequest(joint=joint_command))

    def get_position(self):
        return self.joint_state.position

    async def set_position(self, position: float, velocity: float):
        joint_command = rofi_pb.JointCommand(
            joint_id=self.joint_id,
            command=rofi_pb.JointCommandType.SET_JOINT_POSITION,
            set_position=rofi_pb.JointSetPosition(position=position, velocity=velocity),
        )
        await self.write_command(rofi_pb.RofiRequest(joint=joint_command))

    def get_torque(self):
        return self.joint_state.torque

    async def set_torque(self, torque: float):
        joint_command = rofi_pb.JointCommand(
            joint_id=self.joint_id,
            command=rofi_pb.JointCommandType.SET_JOINT_TORQUE,
            set_torque=torque
        )
        await self.write_command(rofi_pb.RofiRequest(joint=joint_command))


class RofiConnector(RofiCmd):
    def __init__(self, rofi: "Rofi", connector_id: int):
        super().__init__(rofi, rofi_pb.CommandTypeRequest.CONNECTOR)
        self.connector_id = connector_id
        self.connector_state = rofi_pb.Connector()

    def get_position(self):
        return self.connector_state.position

    async def connect(self):
        connector_command = rofi_pb.ConnectorCommand(
            connector_id=self.connector_id,
            command=rofi_pb.ConnectorCommandType.SET_CONNECT
        )
        await self.write_command(rofi_pb.RofiRequest(connector=connector_command))

    async def disconnect(self):
        connector_command = rofi_pb.ConnectorCommand(
            connector_id=self.connector_id,
            command=rofi_pb.ConnectorCommandType.SET_DISCONNECT
        )
        await self.write_command(rofi_pb.RofiRequest(connector=connector_command))

    async def connect_power(self):
        connector_command = rofi_pb.ConnectorCommand(
            connector_id=self.connector_id,
            command=rofi_pb.ConnectorCommandType.CONNECT_POWER
        )
        await self.write_command(rofi_pb.RofiRequest(connector=connector_command))

    async def disconnect_power(self):
        connector_command = rofi_pb.ConnectorCommand(
            connector_id=self.connector_id,
            command=rofi_pb.ConnectorCommandType.DISCONNECT_POWER
        )
        await self.write_command(rofi_pb.RofiRequest(connector=connector_command))

    async def set_distance_mode(self, mode: rofi_pb.LidarDistanceMode):
        connector_command = rofi_pb.ConnectorCommand(
            connector_id=self.connector_id,
            command=rofi_pb.ConnectorCommandType.SET_DISTANCE_MODE,
            set_distance_mode=mode,
        )
        await self.write_command(rofi_pb.RofiRequest(connector=connector_command))


type RofiStateUpdateCallback = Callable[["Rofi", int], None] | None
type RofiStateUpdateCallbackError = Callable[["Rofi", int, str], None] | None
type RofiDisconnectCallback = Callable[[], None] | None


class Rofi:
    def __init__(self, rofi_id: int, ble_device: BLEDevice, try_reconnect: bool = False,
                 wait_for_response: bool = True):
        self.rofi_id = rofi_id
        self.client = BleakClient(ble_device)
        self.try_reconnect = try_reconnect
        self.wait_for_response = wait_for_response
        self.packet_id = 0
        self._rofi_state = rofi_pb.RofiState()
        self._joints = [RofiJoint(self, joint_id) for joint_id in range(3)]
        self._connectors = [
            RofiConnector(self, connector_id) for connector_id in range(6)
        ]
        self._device = RofiDevice(self)
        self._message = RofiMessage(self)
        self._on_disconnect_callback: RofiDisconnectCallback = None
        self._update_rofi_state_callback: RofiStateUpdateCallback = None
        self._update_rofi_state_error_callback: RofiStateUpdateCallbackError = None

    async def connect(self):
        if self.client.is_connected:
            logger.warning("RoFI device is already connected.")
            return
        await self.client.connect()
        await self.client.start_notify(
            CHARACT_UUID_ROFI_STATE, self._update_rofi_state
        )

    async def disconnect(self):
        await self.client.stop_notify(CHARACT_UUID_ROFI_STATE)
        await self.client.disconnect()
        if self._on_disconnect_callback:
            self._on_disconnect_callback()

    async def reconnect(self):
        self.client = BleakClient(self.client.address)
        await self.connect()

    def _update_rofi_state(
            self, characteristic: BleakGATTCharacteristic, data: bytearray
    ):
        self._rofi_state = rofi_pb.RofiState().parse(bytes(data))
        logger.info(
            f"RoFI -> ID: {self._rofi_state.rofi_id}, PackedID: {self._rofi_state.packet_id}"
        )

        self.rofi_id = self._rofi_state.rofi_id

        if self._rofi_state.type == rofi_pb.RofiStateType.STATE_ERROR:
            if self._update_rofi_state_error_callback:
                self._update_rofi_state_error_callback(self, self._rofi_state.packet_id, self._rofi_state.error_message)

            raise RofiException(f"Error: {self._rofi_state.error_message}")

        rofi_state_data = self._rofi_state.state_data

        logger.warning(f"RoFI connectors: {len(rofi_state_data.connectors)}")

        if len(rofi_state_data.connectors) != 0:
            assert len(rofi_state_data.connectors) == len(
                self._connectors
            ), f"Received different number of connectors than configured ({len(rofi_state_data.connectors)} != {len(self._connectors)})"

            for connector_id, connector_state in enumerate(rofi_state_data.connectors):
                if connector_id >= len(self._connectors):
                    raise RofiException("Trying to access non-existing connector")
                self._connectors[connector_id].connector_state = connector_state

        if len(rofi_state_data.joints) != 0:
            assert len(rofi_state_data.joints) == len(
                self._joints
            ), f"Received different number of joints than configured ({len(rofi_state_data.joints)} != {len(self._joints)})"

            for joint_id, joint_state in enumerate(rofi_state_data.joints):
                if joint_id >= len(self._joints):
                    raise RofiException("Trying to access non-existing joint")
                self._joints[joint_id].joint_state = joint_state

        if self._rofi_state.type == rofi_pb.RofiStateType.STATE_SUCCESS and self._update_rofi_state_callback:
            self._update_rofi_state_callback(self, self._rofi_state.packet_id)

    def set_on_disconnect_callback(self, callback: RofiDisconnectCallback):
        self._on_disconnect_callback = callback

    def set_on_update_rofi_state_callback(self, callback: RofiStateUpdateCallback):
        self._update_rofi_state_callback = callback

    def set_on_update_rofi_state_error_callback(self, callback: RofiStateUpdateCallbackError):
        self._update_rofi_state_error_callback = callback

    def get_id(self) -> int:
        return self.rofi_id

    def get_joint(self, joint_id) -> RofiJoint:
        if joint_id < 0 or joint_id >= len(self._joints):
            raise RofiNoDataException(
                f"Invalid joint ID: {joint_id} or no data received"
            )
        return self._joints[joint_id]

    def get_connector(self, connector_id) -> RofiConnector:
        if connector_id < 0 or connector_id >= len(self._connectors):
            raise RofiNoDataException(
                f"Invalid connector ID: {connector_id} or no data received"
            )
        return self._connectors[connector_id]

    def get_device(self) -> RofiDevice:
        return self._device

    def send_message(self, message: str):
        return self._message.send_message(message)
