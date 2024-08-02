# Set RoFI ID example

The example shows RoFI ID configurator which uses IDF console to set RoFI ID.

## Commands

The example provides the following commands:
- `set-id <id>` - sets RoFI ID to `<id>`.
- `get-id` - gets RoFI ID.

ID is stored in ESP32 NVS under namespace `rofi` and key `id`.
