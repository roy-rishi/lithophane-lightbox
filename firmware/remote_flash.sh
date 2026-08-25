# Remotely flash an ESP32 SoC

BUILD_DIR="./build"
REMOTE="pi@rpi.local"
ESPTOOL_VENV="/home/pi/esptool/"

# exit immediately on failure
set -e

# copy .bin files and flash_args to remote
cd "$BUILD_DIR"
scp bootloader/bootloader.bin partition_table/partition-table.bin firmware.bin flash_args $REMOTE:$ESPTOOL_VENV

# run flash utility on remote
ssh "$REMOTE" "cd '$ESPTOOL_VENV' && ./bin/python -m esptool --chip esp32 -p /dev/ttyUSB0 -b 460800 --before=default-reset --after=hard-reset write-flash @flash_args"
