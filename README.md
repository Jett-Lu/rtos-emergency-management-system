# FreeRTOS Emergency Control System  
### Real-Time Multitasking Embedded System on NXP MCU

<img width="1100" height="520" alt="image" src="https://github.com/user-attachments/assets/e1e89499-372c-442f-93dd-1ef8f58b313f" />

This diagram illustrates the interrupt-driven state transitions between normal operation and the emergency handling state.

## Overview

This project implements a deterministic real-time embedded control system using FreeRTOS on an NXP microcontroller. The system simulates a safety-critical emergency workflow in which interrupt-driven events preempt normal system operation and enforce strict task sequencing.

The design emphasizes preemptive scheduling, ISR-safe kernel interaction, hardware timer integration, and priority-based emergency handling under real-time constraints.

## System Operation

### Normal Operation State

- GREEN LED ON  
- RED LED OFF  
- Alarm output (PTC16) LOW  
- FreeRTOS software timer executes every 2 seconds  
- Task1 processes sensor data  
- Task2 updates system display  

Execution sequence:

Software Timer callback → Task1 → Task2

Binary semaphores enforce strict ordering between tasks.

### Emergency State (Triggered by SW2 Interrupt)

- Software timer stopped from ISR context  
- Task3 activated at higher priority  
- GREEN LED OFF  
- Alarm output HIGH  
- RED LED blinking at 4 Hz via PIT hardware interrupt  
- Task1 and Task2 halted  

The system remains halted in this deterministic emergency condition until SW3 is pressed.

### Recovery State (Triggered by SW3 Interrupt)

- PIT timer stopped  
- RED LED OFF  
- GREEN LED ON  
- Alarm output LOW  
- Software timer restarted from ISR  
- Normal task execution resumes  

## State Transitions

Normal Operation  
&nbsp;&nbsp;&nbsp;&nbsp;↓ SW2 Interrupt  
Emergency State  
&nbsp;&nbsp;&nbsp;&nbsp;↓ SW3 Interrupt  
Normal Operation

State transitions are driven exclusively by hardware interrupts to maintain deterministic behavior.

## Architecture

Core components:

- FreeRTOS Software Timer (2 second period)
- Task1 – Sensor Data Processing
- Task2 – Display Update
- Task3 – Emergency Protocol Handler
- SW2 ISR – Emergency trigger
- SW3 ISR – Recovery trigger
- PIT Channel 0 ISR – 4 Hz LED control

## Concurrency Design

Binary semaphores enforce strict execution order:

- Timer callback releases Task1
- Task1 releases Task2
- SW2 ISR releases Task3
- SW3 ISR restores normal scheduling

Task3 is assigned higher priority to guarantee emergency preemption.

Interrupt priorities are configured to respect:

configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

ISR-safe APIs used:

- xTimerStopFromISR
- xTimerStartFromISR
- xSemaphoreGiveFromISR

This ensures safe interaction between interrupt context and the FreeRTOS scheduler.

## Real-Time Characteristics

### Deterministic Halt Condition

Stopping the software timer ensures Task1 and Task2 are no longer scheduled. This creates a true halted state for normal system logic while emergency handling remains active.

### Hardware-Timed Emergency Signaling

The RED LED blinking frequency is generated using PIT Channel 0 interrupts at:

- 125 ms ON  
- 125 ms OFF  
- 4 Hz total frequency  

Blinking is hardware-driven to ensure timing precision independent of task scheduling.

### Preemptive Emergency Handling

Emergency logic executes with elevated priority, guaranteeing that critical behavior overrides standard processing tasks.

## Design Decisions

### Why binary semaphores were used

Binary semaphores enforce a strict execution order between the periodic timer logic and the processing tasks. The design intentionally chains execution as:

Software Timer callback → Task1 → Task2

This prevents tasks from running out of sequence and simplifies verification of system behavior.

### Why Task3 has higher priority

Task3 represents emergency handling logic and must preempt all normal work. Assigning it higher priority ensures deterministic response immediately after SW2 is pressed.

### Why the software timer is stopped from the SW2 ISR

The software timer drives the normal operational cycle. Stopping it from ISR context guarantees that no new normal-operation cycles begin during an emergency state, creating a clean and predictable halt condition.

### Why PIT interrupt is used for 4 Hz blinking

Blinking is generated using hardware timer interrupts to maintain accurate timing independent of task scheduling. This avoids jitter that could occur with task-based delays.

## Timing Overview

| Component              | Timing Behavior             |
||-|
| Software Timer         | 2 second periodic callback |
| PIT Interrupt          | 4 Hz LED toggle            |
| Emergency Activation   | Immediate via ISR          |
| System Resume          | Immediate via ISR          |

The system avoids long blocking delays inside ISRs and maintains strict separation between interrupt context and task context.

## Hardware Platform

- NXP microcontroller (MCUXpresso environment)
- Onboard RED and GREEN LEDs
- Pushbuttons SW2 and SW3
- External LED connected to PTC16 (alarm output)

## Build Environment

- MCUXpresso IDE
- FreeRTOS Kernel
- NXP SDK drivers for GPIO and PIT

This repository includes application source and configuration files. Build artifacts and auto-generated IDE files are intentionally excluded.

## Usage

### Build and Flash (MCUXpresso IDE)

1. Open MCUXpresso IDE.
2. Import the project:
   - File → Import → Existing Projects into Workspace
   - Select the project directory.
3. Confirm board configuration:
   - SW2, SW3, RED/GREEN LEDs, PTC16, and PIT Channel 0 should be configured.
4. Build the project:
   - Project → Build Project
5. Connect the NXP development board via USB.
6. Flash and run:
   - Run → Debug Configurations
   - Select the appropriate debug probe
   - Click Debug, then Resume execution.

### Expected Runtime Behavior

Normal mode:
- GREEN LED ON, RED LED OFF, PTC16 LOW
- Console prints repeating sequence:
  - Software Timer callback message
  - Task1 message
  - Task2 message

Emergency mode (press SW2):
- Console prints emergency shutdown message
- Software timer stops, Task1 and Task2 halt
- Task3 prints emergency protocol messages
- GREEN LED OFF, PTC16 HIGH
- RED LED blinks at 4 Hz

Recovery (press SW3):
- Console prints resume message
- PIT stops blinking, RED LED OFF
- GREEN LED ON, PTC16 LOW
- Software timer restarts and normal sequence resumes
