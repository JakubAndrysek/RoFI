import click
from rofi_connector import RofiConnector
import asyncio
import demo_qt as demo_qt
from demo_code import demo_reboot


# Create a new instance of the Connector class
rofi_connector = RofiConnector()


@click.group()
def cli():
    """RoFI BLE Connector CLI."""
    pass


@click.command()
@click.option("--force-all", is_flag=True, help="Force to list all devices.")
def list(force_all):
    """List all available RoFI devices."""
    rofi_devices = asyncio.run(rofi_connector.scan(force_all))
    if not rofi_devices:
        click.echo("No RoFI devices found")
        return
    for idx, device in enumerate(rofi_devices):
        name = device.name or "Unknown"
        click.echo(f"Device {idx}: {name:>30} ({device.address})")


@click.command()
@click.option("--id", default=0, help="ID to connect to.")
def connect(id):
    """Connect to a resource with a given ID."""
    click.echo(f"Connecting to RoFI device with ID: {id}")
    asyncio.run(rofi_connector.connect(id))
    # rofi = rofi_connector.connect(id)


@cli.command()
def gui():
    """Run the GUI."""
    click.echo("Running the GUI...")
    app = demo_qt.QApplication([])

    event_loop = demo_qt.QEventLoop(app)
    asyncio.set_event_loop(event_loop)

    ex = demo_qt.RofiGUI()
    ex.show()

    with event_loop:
        event_loop.run_forever()


@cli.command()
@click.option("--id", default=0, help="ID of the device to reboot.")
def reboot(id: int = 0):
    """Connect to device and reboot it."""
    click.echo("Rebooting the device...")
    try:
        asyncio.run(demo_reboot(id))
    except Exception as e:
        click.echo(f"Error: {e}")

    click.echo("Rebooted the device.")


# Add commands to the main cli group
cli.add_command(connect)
cli.add_command(list)

if __name__ == "__main__":
    cli()
