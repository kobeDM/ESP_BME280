#!/usr/bin/env python3

import argparse
import os
import signal
import subprocess
import sys


def find_matching_pids(pattern):
    result = subprocess.run(
        ["ps", "-eo", "pid=,args="],
        check=True,
        text=True,
        capture_output=True,
    )

    current_pid = os.getpid()
    matches = []

    for line in result.stdout.splitlines():
        line = line.strip()
        if not line:
            continue

        pid_text, _, command = line.partition(" ")
        if not pid_text or not command:
            continue

        try:
            pid = int(pid_text)
        except ValueError:
            continue

        if pid == current_pid:
            continue

        if pattern in command and "python" in command:
            matches.append((pid, command))

    return matches


def main():
    parser = argparse.ArgumentParser(
        description="Kill the ESP_BME280 serial reader process."
    )
    parser.add_argument(
        "-p",
        "--pattern",
        default="ESP_BME280-ADS1115_serial_influx.py",
        help="Process command-line pattern to match.",
    )
    parser.add_argument(
        "-f",
        "--force",
        action="store_true",
        help="Send SIGKILL instead of SIGTERM.",
    )

    args = parser.parse_args()
    matches = find_matching_pids(args.pattern)

    if not matches:
        print(f"No matching process found for pattern: {args.pattern}")
        return 1

    sig = signal.SIGKILL if args.force else signal.SIGTERM

    for pid, command in matches:
        os.kill(pid, sig)
        print(f"Sent {sig.name} to PID {pid}: {command}")

    return 0


if __name__ == "__main__":
    sys.exit(main())