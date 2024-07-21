import click
from connector import Connector
import asyncio


# Create a new instance of the Connector class
rofi = Connector()


@click.group()
def cli():
    """RoFI BLE Connector CLI."""
    pass


@click.command()
@click.option("--force-all", is_flag=True, help="Force to list all devices.")
def list(force_all):
    """List all available RoFI devices."""
    rofi_devices = asyncio.run(rofi.scan(force_all))
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
    asyncio.run(rofi.connect(id))


# Add commands to the main cli group
cli.add_command(connect)
cli.add_command(list)

if __name__ == "__main__":
    cli()
