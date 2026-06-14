#!/usr/bin/env python3

"""
LX200 Alt-Az Sky Scanner

Scans a rectangular region of the sky using an LX200-compatible Alt-Az mount,
collecting grayscale sensor data at each position and displaying it live.

Features:
- Serpentine raster scanning
- Live matplotlib display
- PNG image saving
- Retry handling
- Slew completion polling
- Ctrl+C safe interruption
- Progress bar + ETA

Dependencies:
    pip install pyserial numpy matplotlib tqdm

Example:
    python3 sky_scan.py \
        --alt1 60 --az1 100 \
        --alt2 30 --az2 160 \
        --rows 10 --cols 10
"""

import argparse
import serial
import time
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
from datetime import datetime
import sys


# =========================
# Configuration
# =========================

#SERIAL_PORT = "/dev/ttyUSB0"
SERIAL_PORT = "/dev/pts/6"
BAUD_RATE = 9600

POLL_INTERVAL = 0.2
MOVE_TIMEOUT = 30.0
HOME_TIMEOUT = 30.0
SETTLING_TIME = 0.5
MAX_RETRIES = 3

SERIAL_TIMEOUT = 1.0


# =========================
# Utility Functions
# =========================

def round_to_nearest_minute(degrees_float):
    """
    Round decimal degrees to nearest arcminute.

    Returns:
        (degrees, minutes)
    """
    sign = 1 if degrees_float >= 0 else -1
    abs_deg = abs(degrees_float)

    degrees = int(abs_deg)
    minutes = round((abs_deg - degrees) * 60)

    # Handle rollover
    if minutes == 60:
        degrees += 1
        minutes = 0

    return sign * degrees, minutes


def format_alt_command(alt):
    """
    Format LX200 altitude command:
    :Sa+DD*MM#
    """
    deg, mins = round_to_nearest_minute(alt)

    sign = "+" if deg >= 0 else "-"
    deg_abs = abs(deg)

    return f":Sa{sign}{deg_abs:02d}*{mins:02d}#"


def format_az_command(az):
    """
    Format LX200 azimuth command:
    :SzDDD*MM#
    """
    deg, mins = round_to_nearest_minute(az)

    deg %= 360

    return f":Sz{deg:03d}*{mins:02d}#"


# =========================
# LX200 Communication
# =========================

class LX200Mount:

    def __init__(self, port, baudrate):
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            timeout=SERIAL_TIMEOUT
        )

    def close(self):
        self.ser.close()

    def send(self, command):
        self.ser.write(command.encode("ascii"))

    def read_until_hash(self):
        return self.ser.read_until(b"#").decode("ascii", errors="ignore")

    def flush(self):
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()

    def home_alt_az(self):
        start_time = time.time()

        self.flush()

        self.send(":GA#") # Send ONE GA request

        while True: # Wait until we actually receive data`
            if (time.time() - start_time) > HOME_TIMEOUT:
                raise TimeoutError("Mount home timeout")

            response = self.read_until_hash().strip("#").strip()

            if response == "": # No data received yet
                continue

            break # Got data or serial timeout, stop waiting

        if response != "":
            return # Return if data was received

        self.wait_for_slew_complete()

        self.flush()

        self.send(":GZ#") # Send ONE GA request

        while True: # Wait until we actually receive a byte
            if (time.time() - start_time) > HOME_TIMEOUT:
                raise TimeoutError("Mount home timeout")

            response = self.read_until_hash().strip("#").strip()

            if response == "": # No data received yet
                continue

            break # Got data or serial timeout, stop waiting

        if response != "":
            return # Return if data was received

        self.wait_for_slew_complete()



    def set_target(self, alt, az):
        alt_cmd = format_alt_command(alt)
        az_cmd = format_az_command(az)

        self.send(alt_cmd)
        time.sleep(0.05)

        self.flush()

        response = self.ser.read(1) #1 if movement is valid. todo

        self.send(az_cmd)
        time.sleep(0.05)

        self.flush()

        response = self.ser.read(1) #1 if movement is valid. todo

    def start_slew(self):
        self.send(":MA#")
        time.sleep(0.05)

        self.flush()

        response = self.ser.read(1) #1 if movement is valid. todo

    def wait_for_slew_complete(self):
        start_time = time.time()

        while True:
            if (time.time() - start_time) > MOVE_TIMEOUT:
                raise TimeoutError("Mount slew timeout")

            self.send(":D#") # Send ONE status request

            while True: # Wait until we actually receive a byte
                if (time.time() - start_time) > MOVE_TIMEOUT:
                    raise TimeoutError("Mount slew timeout")

                response = self.ser.read(1)

                if response == b"": # No byte received yet
                    continue

                break # Got a byte, stop waiting

            if response == b"#": # '#' = still moving
                time.sleep(POLL_INTERVAL)
                continue

            if response == b"\x00": # NULL = movement complete
                return

            time.sleep(POLL_INTERVAL) # Ignore unexpected bytes

    def move_to(self, alt, az):
        for attempt in range(MAX_RETRIES):

            try:
                self.flush()

                self.set_target(alt, az)
                self.start_slew()

                self.wait_for_slew_complete()

                time.sleep(SETTLING_TIME)

                return

            except Exception as e:
                print(f"\nMove retry {attempt + 1}/{MAX_RETRIES}: {e}")

                if attempt == MAX_RETRIES - 1:
                    raise

    def read_sensor(self):
        for attempt in range(MAX_RETRIES):

            try:
                self.flush()

                self.send(":Gl#")
                time.sleep(0.05)

                response = self.read_until_hash().strip("#").strip()

                if len(response) != 3:
                    raise ValueError(f"Invalid response length: {response}")

                value = int(response)

                if not (0 <= value <= 255):
                    raise ValueError(f"Value out of range: {value}")

                return value

            except Exception as e:
                print(f"\nSensor retry {attempt + 1}/{MAX_RETRIES}: {e}")

                if attempt == MAX_RETRIES - 1:
                    raise

                time.sleep(0.2)


# =========================
# Scan Coordinate Generator
# =========================

def generate_scan_grid(alt1, az1, alt2, az2, rows, cols):
    altitudes = np.linspace(alt1, alt2, rows)
    azimuths = np.linspace(az1, az2, cols)

    grid = []

    for row in range(rows):

        alt = altitudes[row]

        if row % 2 == 0:
            az_sequence = azimuths
        else:
            az_sequence = azimuths[::-1]

        row_points = []

        for col, az in enumerate(az_sequence):
            row_points.append((row, col, alt, az))

        grid.extend(row_points)

    return grid


# =========================
# Main Scan Function
# =========================

def run_scan(args):
    rows = args.rows
    cols = args.cols

    image_data = np.zeros((rows, cols), dtype=np.uint8)

    scan_points = generate_scan_grid(
        args.alt1,
        args.az1,
        args.alt2,
        args.az2,
        rows,
        cols
    )

    print("\nScan Summary")
    print("============")
    print(f"Top-left     : Alt {args.alt1}°, Az {args.az1}°")
    print(f"Bottom-right : Alt {args.alt2}°, Az {args.az2}°")
    print(f"Resolution   : {rows} x {cols}")
    print(f"Total points : {len(scan_points)}")
    print()

    confirm = input("Begin scan? [y/N]: ").strip().lower()

    if confirm != "y":
        print("Scan cancelled.")
        return

    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    output_filename = f"scan_{timestamp}.png"

    mount = LX200Mount(SERIAL_PORT, BAUD_RATE)

    plt.ion()

    fig, ax = plt.subplots()

    im = ax.imshow(
        image_data,
        cmap="gray",
        vmin=0,
        vmax=255,
        origin="upper"
    )

    ax.set_title("Live Sky Scan")
    ax.set_xlabel("Azimuth")
    ax.set_ylabel("Altitude")

    plt.colorbar(im, ax=ax)

    plt.show(block=False)

    mount.home_alt_az();

    try:
        with tqdm(total=len(scan_points), unit="pt") as pbar:
            for row, col, alt, az in scan_points:
                # Correct displayed column for serpentine rows
                display_col = col if row % 2 == 0 else (cols - 1 - col)

                pbar.set_description(
                    f"Alt={alt:.2f} Az={az:.2f}"
                )

                mount.move_to(alt, az)

                value = mount.read_sensor()

                image_data[row, display_col] = value

                im.set_data(image_data)

                fig.canvas.draw()
                fig.canvas.flush_events()

                pbar.update(1)

    except KeyboardInterrupt:
        print("\n\nScan interrupted by user.")

    finally:

        plt.ioff()

        plt.figure(fig.number)

        plt.imshow(
            image_data,
            cmap="gray",
            vmin=0,
            vmax=255,
            origin="upper"
        )

        plt.title("Final Sky Scan")

        plt.savefig(output_filename, dpi=300)

        print(f"\nSaved image: {output_filename}")

        mount.close()

        plt.show()


# =========================
# Entry Point
# =========================

def main():
    parser = argparse.ArgumentParser(
        description="LX200 Alt-Az Sky Scanner"
    )

    parser.add_argument("--alt1", type=float, required=True,
                        help="Top-left altitude")

    parser.add_argument("--az1", type=float, required=True,
                        help="Top-left azimuth")

    parser.add_argument("--alt2", type=float, required=True,
                        help="Bottom-right altitude")

    parser.add_argument("--az2", type=float, required=True,
                        help="Bottom-right azimuth")

    parser.add_argument("--rows", type=int, required=True,
                        help="Number of vertical scan points")

    parser.add_argument("--cols", type=int, required=True,
                        help="Number of horizontal scan points")

    args = parser.parse_args()

    run_scan(args)


if __name__ == "__main__":
    main()
