import asyncio
import logging
import time

import coloredlogs
from bleak import BleakScanner
from bleak.backends.device import BLEDevice

from rofi_angle import Angle
from rofi_device import Rofi

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
coloredlogs.install(
    level="INFO", fmt="%(asctime)s %(levelname)s %(message)s", datefmt="%H:%M:%S"
)


class RofiConnectorException(Exception):
    pass


class RofiConnector:
    def __init__(self, try_reconnect: bool = True, wait_for_response: bool = True):
        self._devices = []
        self._try_reconnect = try_reconnect
        self._wait_for_response = wait_for_response

    def _filter_devices(self, rofi_id: int | None = None):
        # Filter devices by name starting with "RoFI-" + optional ID
        if rofi_id is None:
            return [
                device
                for device in self._devices
                if device.name and device.name.startswith("RoFI-")
            ]
        else:
            return [
                device
                for device in self._devices
                if device.name and device.name.startswith(f"RoFI-{rofi_id}")
            ]

    async def scan(
        self, rofi_id: None | int = None, force_show_all: bool = False
    ) -> list[BLEDevice]:
        """Scan for RoFI devices."""
        logger.info("Scanning for devices...")
        self._devices = await BleakScanner.discover()

        if not force_show_all:
            self._devices = self._filter_devices(rofi_id)

        return self._devices

    async def connect(self, rofi_id: int) -> Rofi:
        """Connect to a RoFI device with a given ID."""
        try:
            self._devices = await self.scan(rofi_id)
            if not self._devices:
                logger.error(f"No RoFI device with ID {rofi_id} found.")
                raise RofiConnectorException(f"No RoFI device with ID {rofi_id} found.")
            if len(self._devices) > 1:
                logger.warning(f"Multiple RoFI devices with same ID {rofi_id} found.")
                logger.info("Connect to the first one.")
            device = self._devices[0]

            logger.info(f"Connecting to {device.name} ({device.address})")
            rofi_device = Rofi(
                rofi_id, device, self._try_reconnect, self._wait_for_response
            )
            await rofi_device.connect()
            return rofi_device
        except Exception as e:
            logger.error(f"Connector Error: {e}")
            raise RofiConnectorException(f"Connector Error: {e}") from e
