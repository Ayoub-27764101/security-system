# Security System

A home security monitor built on an **Arduino UNO**. It watches for flame, motion and gas, measures temperature, pressure and altitude, sounds a buzzer when something is wrong, and reports everything to a **Blynk** dashboard. The project is simulated in **Proteus**.

## Features

- Flame, motion (PIR) and gas detection
- Temperature, pressure and altitude from a BMP180 sensor
- Buzzer alarm with two levels: low tone for one alert, high tone for two or more
- Live dashboard in the Blynk app, refreshed every 2 seconds

## Hardware

| Part | Arduino pin | Type |
|---|---|---|
| Gas sensor | A0 | analog input |
| PIR motion sensor | A1 | analog input |
| Buzzer | A2 | output |
| Flame sensor | D10 | digital input |
| BMP180 | A4 (SDA), A5 (SCL) | I2C |

## Blynk dashboard

| Virtual pin | Value |
|---|---|
| V0 | Flame detected (0 / 1) |
| V1 | Motion detected (0 / 1) |
| V2 | Gas detected (0 / 1) |
| V3 | Temperature (°C) |
| V4 | Pressure (hPa) |
| V5 | Altitude (m) |

The board talks to Blynk over the **serial port** (`BlynkSimpleStream`), not WiFi. In Proteus this goes through the COMPIM serial component.

## Alarm logic

| Situation | Buzzer |
|---|---|
| No alert | silent |
| One alert | 600 Hz |
| Two or three alerts | 1200 Hz |

Motion counts as detected when the PIR reading is 650 or more, and gas when the reading is 620 or more (on the 0 to 1023 analog scale). Change these thresholds in `sss/sss.ino` to suit your sensors.

## Repository contents

| File | What it is |
|---|---|
| `sss/sss.ino` | Arduino sketch |
| `DHT22.pdsprj` | Proteus simulation project |
| `FlameSensorTEP.HEX`, `GasSensorTEP.HEX`, `PIRSensorTEP.HEX` | Firmware files for the sensor models in Proteus |

## Getting started

1. Install the **Arduino IDE**.
2. In the Library Manager, install **Blynk** and **Adafruit BMP085 Library**.
3. Create a device in the Blynk console and copy its auth token.
4. Open `sss/sss.ino` and put your template ID, template name and auth token at the top.
5. Compile the sketch and export the compiled binary (`.hex`).
6. Open `DHT22.pdsprj` in Proteus, load your `.hex` into the Arduino UNO, and point each sensor model to its `.HEX` file from this repository.
7. Run the simulation.

## Security note

The Blynk auth token is a secret: anyone who has it can read and write your device's data. Keep your own token out of public repositories, and regenerate it in the Blynk console if it has been published.
