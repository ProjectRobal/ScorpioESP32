import numpy as np
import ble_serial
from ble_serial.scan import main as scanner
from ble_serial.bluetooth.ble_interface import BLE_interface
import asyncio

import serial

from matplotlib import pyplot
from matplotlib.animation import FuncAnimation

TARGET="Scorpio"

SERIAL="/dev/ttyACM0"

BAUDRATE=115200

ADAPTER="hci0"

SCAN_TIME=5

BUFFER_SIZE=4096

AMPLITUDE=3.3

MAX_RESOLUTION=4095

class Channel:
    def __init__(self,ax):
        self._buffer=np.array([],np.float32)
        self._ax=ax
        self._points,=ax.plot([],[])
    
    def apppend(self,byte):
        if self._buffer.size>=BUFFER_SIZE:
            self._buffer=np.append([])
            return

        sample=(byte/MAX_RESOLUTION)*AMPLITUDE

        self._buffer=np.append(self._buffer,sample)

    def update(self,fig):
            self._points.set_data(np.arange(0,self._buffer.size),self._buffer)


figure=pyplot.figure()

ax1=figure.add_subplot(311)
ax2=figure.add_subplot(312)
ax3=figure.add_subplot(313)

channels=[
    Channel(ax1),
    Channel(ax2),
    Channel(ax3)
]

curr_channel=0


def recive_callback(value: bytes):
    global curr_channel
    if chr(value[0])=='c':
        curr_channel=int(chr(value[1]))
    else:
        try:
            print(int(value[0:len(value)-6]))
            channels[curr_channel].apppend(int(value[0:len(value)-6]))
            
        except:
            pass

async def ble_main():

    devices=await scanner.scan(ADAPTER,SCAN_TIME,None)

    print()
    scanner.print_list(devices)

    target_addr=None

    for dev in devices:
        if dev.name==TARGET:
            target_addr=dev.address
            break

    if target_addr is None:
        print("Cannot find device with name: ",TARGET)
        return
    
    print("Found device: ",TARGET," with address: ",target_addr)

    ble=BLE_interface(ADAPTER)

    try:
        await ble.connect(target_addr,"public",10.0)
        await ble.setup_chars(None,None,"r")

        await asyncio.gather(ble.send_loop())
    finally:
        await ble.disconnect()


def serial_loop():

    global outputs
    #pyplot.show(block=False)

    with serial.Serial(SERIAL,BAUDRATE,timeout=1) as ser:
        while True:
            recive_callback(ser.readline())

            for channel in channels:
                channel.update(figure)
            
            #pyplot.draw()
            pyplot.pause(0.001)

        

async def main():

    pyplot.show()

    serial_loop()

if __name__ == "__main__":
    asyncio.run(main())