The hardware setup for my kitchen:

1. NodeMCUv2 (from AZ-Delivery)
2. 2 x 50cm WS2812b Neopixel strip
3. HC-SR501 PIR-Sensor
4. 5V 1A Power supply
5. Wires (obviously)

Software used:

1. Visual Studio Code
2. WLED 0.13.3
    -   PIR usermod
3. PlatformIO in VS Code (for compiling)
    -   Tutorial https://www.youtube.com/watch?v=fRxyp5Yhivc&t=5474s

Settings in WLED:

1. max. 850mA (because of 1A output - depends on power supply)
2. nighttime only