# jonas_demo

Raspberry Pico (RP2040) that drives 4 VL53L0X Time of Flight sensors

Sensor driver code snagged from https://github.com/Imel23/VL53L0X-pico

# Dependencies
- cmake (tested on 3.22.1)
- pico-sdk (tested on 2.2.1)

# Installation and Operation Guide
- Clone the `pico-sdk` repo
- `git clone https://github.com/raspberrypi/pico-sdk.git`
- Add the following line to your .bashrc (located in $HOME/.bashr)
- `export PICO_SDK_PATH="<insert path to sdk>"`
- Then run `source ~/.bashr` to re source your newly edited bashrc
- Clone this repo
- `git clone git@github.com:IsikcanYilmaz/jonas_demo.git`
- Then, in the repo directory:
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`
- At this point the firmware should be compiled.
- While pressing and holding the BOOT button on the pico, plug it in.
- This should introduce the pico as a storage device to your computer, in the path `/media/<user>/RPI-RP2/`
- copy `main_app.uf2` in the build directory into that path
- `cp main_app.uf2 /media/$USER/RPI-RP2/`
- After this point you should be able to read the sensor readings from the serial port `/dev/ttyACM0`

# Pins
- GPIO 4, 5 are SDA and SCL respectively
- GPIOs 14, 15, 17, 16 are each sensors XSHUT signals
- Refer to the pictures in the `/docs/`

# Misc
The sensors share a single I2C bus. Because of this, at boot a procedure is run in order to set each one's address. This is done by the following:

- Have all of the sensors be on the I2C bus, and have each one's XSHUT pin hooked to a GPIO on the pico.
- At boot, first output LOW to every one of those XSHUTs. 
- Then, one by one, pull them HIGH, init the sensor, set it to an address that is 1 + n + DEFAULT (0x29), and keep XSHUT HIGH
- Have some delays in between these steps just in case
