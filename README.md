# MCO556 FreeRTOS Multitasking Emergency Control System

## Project Description (Short)
A FreeRTOS based embedded system demonstrating multitasking, interrupt driven control, and priority based emergency handling on an NXP microcontroller. The project models a safety critical real time system with normal operation, emergency shutdown, and manual recovery using tasks, semaphores, software timers, and hardware timers.

## Overview
This project was developed as a final project for MCO556 (Real Time Systems). It implements an event driven embedded controller with real time constraints using FreeRTOS. The system operates in two modes: regular operation and emergency mode. Emergency handling is designed to immediately preempt all non critical tasks, reflecting real world safety critical embedded systems.

## System Behavior

### Normal Operation
- A FreeRTOS software timer periodically simulates sensor readings
- Task1 processes sensor data
- Task2 updates the display or user interface
- Green LED indicates normal system operation
- Alarm output is OFF

### Emergency Operation
- Triggered by pressing SW2 (GPIO interrupt)
- Software timer is stopped immediately
- Normal operation tasks are halted
- A high priority emergency task preempts the system
- Red LED flashes rapidly using a PIT hardware timer
- External alarm indicator is enabled
- System remains halted until manually cleared

### System Recovery
- Pressing SW3 clears the emergency state
- PIT timer is stopped
- LEDs are reset to normal operation
- Software timer restarts
- Regular operation resumes

---

## Tasks and Priorities

| Task  | Function                         | Priority |
|------|----------------------------------|----------|
| Task1 | Sensor data processing           | Low      |
| Task2 | Display update                   | Low      |
| Task3 | Emergency protocol handling      | High     |

Task3 has higher priority to ensure emergency handling preempts all other system activity.

## FreeRTOS Features Used
- Tasks and preemptive scheduling
- Binary semaphores for synchronization
- Software timers for periodic events
- ISR safe FreeRTOS APIs
- Priority based task preemption
- Hardware timer integration (PIT)

## Interrupts
- SW2 GPIO interrupt triggers emergency shutdown
- SW3 GPIO interrupt clears emergency and resumes operation
- PIT timer interrupt toggles the red LED during emergency mode

## Hardware Indicators
- Green LED: Normal operation
- Red LED: Emergency flashing indicator
- External LED (PTC16): Alarm indicator

## File Structure
- MCO556_Project_Lu.c Main application source file

## Build and Run
- Requires NXP SDK and FreeRTOS
- Configure board pins, clocks, and peripherals using MCUXpresso
- Build and flash to target hardware
- Use a serial console to observe system logs

## Date
December 2023

