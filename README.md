# MCO556 Final Project: Multitasking Embedded System with Real-Time Requirements

## Overview

This project implements a multitasking embedded system using FreeRTOS and hardware interrupts to simulate real-life operations with hard-real-time constraints. The system runs on a FreeRTOS kernel and interacts with the onboard LEDs, push buttons, and external pins for an "alarm state" simulation, making it a highly responsive and interactive system.

## Features

The system handles five core operations:

1. **Sensor Reading** – Managed by a software timer ISR, which triggers every 2 seconds.
2. **Sensor Data Processing** – Handled by Task1, which processes the incoming sensor data.
3. **Display Update** – Managed by Task2, responsible for updating the output display.
4. **Emergency Shutdown** – Triggered by pressing SW2, handled by Task3, and stops regular operations.
5. **System Reboot** – Pressing SW3 resumes normal operation, turning off the alarm and restarting the tasks.

### System Operation Flow

- **Regular Operation**:
    - When the system starts, the onboard **GREEN LED** turns ON (indicating regular operation), and **pin PTC16** stays LOW (alarm OFF).
    - Three tasks run in parallel: sensor reading (via the software timer ISR), sensor data processing (Task1), and display updates (Task2).
    - Console messages are printed to indicate the current operation state:
        - `"Software Timer ISR – sensors are read"`
        - `"Task1 – sensor data is processed"`
        - `"Task2 – display is updated"`

- **Emergency System Shutdown** (Triggered by SW2):
    - Pressing **SW2** triggers an emergency shutdown where:
        - The software timer stops.
        - **Task3** handles the emergency, printing:
            - `"Task3 - EMERGENCY protocol is being executed..."`
            - `"The system is halted"`
            - `"RED LIGHT is flashing and SOUND ALARM is ON"`
            - `"Press button SW3 to clear the system and reboot..."`
        - **GREEN LED** turns OFF, **pin PTC16** goes HIGH (alarm ON), and the **RED LED** starts blinking rapidly at 4Hz (via PIT timer).

- **Resuming Normal Operation** (Triggered by SW3):
    - Pressing **SW3** clears the emergency state and resumes normal operation:
        - The software timer restarts.
        - **RED LED** turns OFF, **GREEN LED** turns back ON, and **pin PTC16** goes LOW (alarm OFF).
        - The regular tasks (sensor reading, data processing, and display updates) resume.

### Programming Highlights

- **FreeRTOS**: The system uses FreeRTOS to manage multiple tasks and ensures real-time performance.
- **Hardware Interrupts**: Onboard buttons **SW2** and **SW3** trigger hardware interrupts for emergency shutdown and system reboot.
- **PIT Timer**: The **RED LED** blinks at 4Hz using the **PIT timer channel 0** interrupts during an emergency state.
- **Semaphores**: Binary semaphores manage task synchronization, ensuring that tasks execute in the correct sequence without conflicts.
- **External LED (PTC16)**: An external LED connected to **pin PTC16** simulates an alarm state by turning ON/OFF based on system status.

## How It Works

1. **Initialization**:
    - The program initializes all peripherals, including buttons, LEDs, timers, and semaphores.
    - Upon startup, the system displays the project name and initializes the task handlers.

2. **Emergency State**:
    - When **SW2** is pressed, the system halts regular tasks, activates the alarm, and blinks the **RED LED** rapidly to signal the emergency.
    - Task3 takes control to handle the emergency protocol until **SW3** is pressed.

3. **System Reset**:
    - After pressing **SW3**, the system resumes normal operation, restarts the software timer, and resets all visual and audio indicators.

## Key Components

- **Software Timer**: Triggers sensor reading every 2 seconds.
- **Task1**: Handles sensor data processing.
- **Task2**: Manages display updates.
- **Task3**: Executes emergency protocols.
- **SW2 ISR**: Stops the system and switches to emergency mode.
- **SW3 ISR**: Restores the system to regular operation.
