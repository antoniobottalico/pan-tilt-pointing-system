# Two-Axis Pointing System (Pan-Tilt) — ESP32

A simple pan-tilt aiming/pointing system built with an ESP32 and two servos, controllable over serial or running in an autonomous demo mode.

## Features

- **Serial control**: send precise pan/tilt angles from the Serial Monitor or any serial terminal
- **Demo mode**: automatic random movement for testing wiring, range of motion, and jitter
- **Angle constraining**: inputs are clamped to a safe 0–180° range
- **Simple text protocol**: easy to integrate with other software (Python, joystick controllers, computer vision targeting, etc.)

## Hardware

| Component        | Pin / Note                        |
|-------------------|------------------------------------|
| Pan servo signal  | GPIO25                             |
| Tilt servo signal | GPIO26                             |
| Servo power       | External 5V supply (not from ESP32 5V pin for high loads) |
| Ground            | Shared between ESP32 and servo supply |

> Tested with SG90/MG90S-type servos (500–2400 µs pulse range, 50 Hz).

## Library dependencies

- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo) (install via Arduino Library Manager)

## Serial Command Protocol

Connect at **115200 baud**, line ending: Newline (`\n`).

| Command           | Description                          | Example       |
|--------------------|---------------------------------------|---------------|
| `P<angle>`         | Set pan servo angle (0–180)           | `P90`         |
| `T<angle>`         | Set tilt servo angle (0–180)          | `T45`         |
| `P<angle> T<angle>`| Set both axes in a single command     | `P90 T45`     |
| `D`                | Toggle demo (random movement) mode    | `D`           |
| `?`                | Print current pan/tilt status         | `?`           |

Any manual `P`/`T` command automatically disables demo mode.

## Example session

```
> P90 T45
Pan: 90 deg | Tilt: 45 deg | Demo mode: OFF

> D
Demo mode enabled

> ?
Pan: 132 deg | Tilt: 77 deg | Demo mode: ON
```

## Repository structure

```
pointing-system/
├── pointing_system/
│   └── pointing_system.ino   # Arduino sketch (open this in Arduino IDE)
├── examples/
│   └── python_serial_client.py  # Optional PC-side control client
├── README.md
├── LICENSE
└── .gitignore
```



## Possible extensions

- Non-blocking, smooth interpolation between angles instead of instant jumps
- Integration with a joystick, gamepad, or computer-vision tracking system
- Add a third axis (roll) or a laser/camera mount
- Wi-Fi or Bluetooth control instead of USB serial

## License

MIT — feel free to use and adapt.
