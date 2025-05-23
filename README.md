# GSM Relay

Control a relay output using a simple phone call. Built with Arduino Nano and SIM900A GSM module.

> **Note:** Parts of this README were generated with the assistance of GitHub Copilot Chat (AI).

## Features
- Remote Relay Control: Activate a relay by calling the device from an authorized phone number.
- Address Book: Only numbers in the address book can trigger the relay. Manage numbers via serial commands or learning mode.
- Status LEDs: Indicates heartbeat and system states.
- CLI over Serial: Add, remove, list, or check numbers using serial commands.

## Hardware Requirements
- Arduino Nano (or compatible)
- SIM900A GSM module
- Relay module
- LEDs (Heartbeat and User)
- Push button

## Usage
1. Wiring:

- Connect GSM module to pins 8 (RX) and 9 (TX).
- Relay control on pin 4.
- Heartbeat LED on pin 13, user LED on pin 3.
- Button on pin 2 (active LOW).

2. Setup:
- Compile the project using PlatformIO.
- Upload the code to your Arduino.
- Open Serial Monitor at 9600 baud to access CLI commands or enter learning mode.

3. CLI Commands:
- `help` — List available commands
- `nrList` — List stored phone numbers
- `nrAdd <index> <number>` — Add a number at position
- `nrAddNext <number>` — Add a number to next available slot
- `nrRemove <index>` — Remove a number
- `nrErase` — Erase all numbers
- `nrCheck <number>` — Check if a number is allowed

4. Learning Mode:
- Hold the button for >5 seconds to enter learning mode.
- The next incoming call adds the number to the address book.

## How it Works
- When the system receives a call, it checks the caller’s number.
- If the number is authorized, the relay is triggered.
- In learning mode, incoming numbers are added to the address book.

> **Note:** You must have a valid SIM card with credit in the GSM module.
