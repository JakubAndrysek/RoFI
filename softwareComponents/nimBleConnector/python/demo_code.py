import asyncio
import logging
import coloredlogs
import time
from rofi_angle import Angle
from rofi_connector import RofiConnector


logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
coloredlogs.install(
    level="INFO", fmt="%(asctime)s %(levelname)s %(message)s", datefmt="%H:%M:%S"
)


async def demo_move_all(id: int = 0):
    rofi_dev = await get_device(id)
    counter = 0
    while True:
        for joint in range(3):
            logger.info(f"Setting servo {joint} position to 0")
            await rofi_dev.get_joint(joint).set_position(Angle.from_deg(0).to_rad(), 1)
            await asyncio.sleep(0.5)

            logger.info(f"Setting servo {joint} position to 20")
            await rofi_dev.get_joint(joint).set_position(Angle.from_deg(1).to_rad(), 1)
            await asyncio.sleep(0.5)
            counter += 1


async def demo_fast_move(id: int = 0):
    rofi_dev = await get_device(id)

    joint_0 = rofi_dev.get_joint(0)
    start_time = asyncio.get_event_loop().time()
    for _ in range(10000000):
        logger.info("Setting position to 0")
        await joint_0.set_position(Angle.from_deg(45).to_rad(), 1)

        await asyncio.sleep(0.1)

        logger.info("Setting position to 20")
        await joint_0.set_position(Angle.from_deg(20).to_rad(), 1)

        await asyncio.sleep(0.1)

    stop_time = asyncio.get_event_loop().time()
    logger.info(f"Time: {stop_time - start_time} s")


async def demo_reboot(id: int = 0):
    rofi_dev = await get_device(id)
    await rofi_dev.get_device().reboot()


async def demo_connector(id: int = 0):
    rofi_dev = await get_device(id)
    servo_id = 0
    while True:
        try:
            logger.info(f"Setting servo ID to {servo_id}")
            await rofi_dev.get_connector(servo_id).connect()
            await time.sleep(1)
            await rofi_dev.get_connector(servo_id).disconnect()
            await time.sleep(1)
        except Exception as e:
            logger.error(f"Error: {e}")

        await asyncio.sleep(1)


async def get_device(id: int = 0):
    rofi_connector = RofiConnector(try_reconnect=False, wait_for_response=True)
    rofi_dev = await rofi_connector.connect(rofi_id=id)
    logger.info(f"RoFI device ID: {rofi_dev.get_id()}")
    return rofi_dev


if __name__ == "__main__":
    # asyncio.run(demo_move())
    # asyncio.run(demo_move_all())
    # asyncio.run(demo_connector())
    asyncio.run(demo_reboot())
