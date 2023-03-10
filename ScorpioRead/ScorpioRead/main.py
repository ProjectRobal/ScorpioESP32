import numpy as np
import ble_serial
from ble_serial.scan import main as scanner
from ble_serial.bluetooth.ble_interface import BLE_interface
import asyncio

import serial

from matplotlib import pyplot
from matplotlib.animation import FuncAnimation
from matplotlib.backends.backend_agg import FigureCanvasAgg

import pygame

TARGET="Scorpio"

SERIAL="/dev/ttyACM0"

BAUDRATE=115200

ADAPTER="hci0"

SCAN_TIME=5

BUFFER_SIZE=4096

AMPLITUDE=3.3

MAX_RESOLUTION=4095.0

SAMP_FREQ=20000.0

class Channel:
    def __init__(self,ax):
        self._buffer=np.array([],np.float32)
        self._buffer=np.zeros(BUFFER_SIZE,np.float32)
        self._buffer[0]=MAX_RESOLUTION
        self._x=np.arange(0,BUFFER_SIZE*(1/SAMP_FREQ),1/SAMP_FREQ)
        self._ax=ax
        self._ax.autoscale(enable=True,axis='y')
        self._points,=ax.plot(self._x,self._buffer)
    
    def apppend(self,byte):

        #sample=(byte/MAX_RESOLUTION)*AMPLITUDE
        print(byte)
        self._buffer=np.roll(self._buffer,-1)
        self._buffer[-1]=byte

        self.update()


    def update(self):
            self._points.set_data(self._x,self._buffer)


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


def render_figure(fig):
    pyplot.draw()
    canvas = FigureCanvasAgg(fig)
    canvas.draw()
    renderer = canvas.get_renderer()
    raw_data = renderer.tostring_rgb()
    size = canvas.get_width_height()
    return pygame.image.fromstring(raw_data, size, "RGB")


def recive_callback(value: bytes):
    
    try:
        line=value.decode()
        (channel,value)=line.split(':')
        channels[int(channel)].apppend(int(value))
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


async def main():

    pyplot.autoscale(enable=True,axis='y')

    ser=serial.Serial(SERIAL,BAUDRATE,timeout=1)

    pygame.init()

    screen = pygame.display.set_mode((800, 600))

    #pyplot.show()
    running = True

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        recive_callback(ser.readline())
    
        screen.fill((0, 0, 0))

        screen.blit(render_figure(figure),(0,0))

        pygame.display.flip()
        #pyplot.pause(0.001)

    pygame.quit()

if __name__ == "__main__":
    asyncio.run(main())