"""
Example Python client for the ESP32 Pan-Tilt Pointing System.

Requires: pyserial  (pip install pyserial)

Usage:
    python python_serial_client.py COM5        (Windows)
    python python_serial_client.py /dev/ttyUSB0  (Linux/Mac)

Once connected, type commands interactively:
    P90 T45   -> move to pan=90, tilt=45
    D         -> toggle demo mode
    ?         -> print status
    quit      -> exit the client
"""

import sys
import time

try:
    import serial
except ImportError:
    print("Missing dependency. Install it with: pip install pyserial")
    sys.exit(1)


def main():
    if len(sys.argv) < 2:
        print("Usage: python python_serial_client.py <serial_port>")
        print("Example: python python_serial_client.py /dev/ttyUSB0")
        sys.exit(1)

    port = sys.argv[1]
    baud_rate = 115200

    print(f"Connecting to {port} at {baud_rate} baud...")
    with serial.Serial(port, baud_rate, timeout=1) as ser:
        time.sleep(2)  # allow ESP32 to reset after opening the serial port
        ser.reset_input_buffer()
        print("Connected. Type commands (P90 T45 | D | ? | quit)\n")

        while True:
            try:
                user_input = input("> ").strip()
            except (EOFError, KeyboardInterrupt):
                print("\nExiting.")
                break

            if user_input.lower() == "quit":
                break
            if not user_input:
                continue

            ser.write((user_input + "\n").encode("utf-8"))
            time.sleep(0.1)

            while ser.in_waiting:
                line = ser.readline().decode("utf-8", errors="ignore").strip()
                if line:
                    print(line)


if __name__ == "__main__":
    main()
