import asyncio
import os

from pprint import pprint
from bleak import BleakClient
from bleak.backends.characteristic import BleakGATTCharacteristic

# Arduino MAC address
ADDRESS = "EF:76:D7:2C:0A:FE"

# Info Characteristic UUID
CHAR_UUID = "d3d46a35-4394-e9aa-5a43-e792112069ed"

DETECT_AMMOUNT = 8

buff = [0] * DETECT_AMMOUNT
index = 0

def notification_handler(characteristic: BleakGATTCharacteristic, data: bytearray):
    """Simple notification handler which prints the data received."""
    global index

    received_int = int.from_bytes(data, byteorder="little")

    # Circ buff impl.
    if index < DETECT_AMMOUNT:
        index = index + 1
    else:
        index = 0
    buff[index-1] = received_int

    # Counting samples
    #  0 -> no fall detected
    # !0 -> model fall prediction in precentage
    state_good = buff.count(0)
    state_alert = DETECT_AMMOUNT - state_good

    # If there are more than half "alert" samples
    # Deploy an messeage
    if state_alert > state_good:
        print(f"FALL DETECTED!!! from {characteristic}")
        os.system('setterm -background red -foreground white')
    else:
        os.system('setterm -background black -foreground white')

    # Debug 
    # print(f"Received: {received_int:#x}")

async def main(address):
    async with BleakClient(ADDRESS) as client:
        await client.start_notify(CHAR_UUID, notification_handler)
        await asyncio.sleep(100.0)
        await client.stop_notify(CHAR_UUID)


asyncio.run(main(ADDRESS))