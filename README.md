# FreeRTOS Emergency Control System  
### Real-Time Multitasking Embedded System on NXP MCU

## Overview

This project implements a deterministic real-time embedded control system using FreeRTOS on an NXP microcontroller. The system simulates a safety-critical emergency workflow in which interrupt-driven events preempt normal system operation and enforce strict task sequencing.

The design emphasizes preemptive scheduling, ISR-safe kernel interaction, hardware timer integration, and priority-based emergency handling under real-time constraints.

---

## System States

The system operates as a two-state real-time controller.

### 1. Normal Operation State

- GREEN LED ON  
- RED LED OFF  
- Alarm output (PTC16) LOW  
- FreeRTOS software timer executes every 2 seconds  
- Task1 processes sensor data  
- Task2 updates system display  

Execution sequence:

Software Timer Callback → Task1 → Task2

Tasks execute in a strictly controlled order using binary semaphores.

---

### 2. Emergency State (Triggered by SW2 Interrupt)

- Software timer stopped from ISR context  
- Task3 activated at higher priority  
- GREEN LED OFF  
- Alarm output HIGH  
- RED LED blinking at 4 Hz via PIT hardware interrupt  
- Task1 and Task2 halted  

The system remains halted in this deterministic emergency condition until SW3 is pressed.

---

### 3. Recovery State (Triggered by SW3 Interrupt)

- PIT timer stopped  
- RED LED OFF  
- GREEN LED ON  
- Alarm output LOW  
- Software timer restarted from ISR  
- Normal task execution resumes  

---

## State Transition Diagram

```
Normal Operation
        |
        | SW2 Interrupt
        v
Emergency State (Timer Stopped, Task3 Active)
        |
        | SW3 Interrupt
        v
Normal Operation (Timer Restarted)
```

State transitions are driven exclusively by hardware interrupts to maintain deterministic behavior.

---

## Architecture

### Core Components

- FreeRTOS Software Timer (2 second period)
- Task1 – Sensor Data Processing
- Task2 – Display Update
- Task3 – Emergency Protocol Handler
- SW2 ISR – Emergency trigger
- SW3 ISR – Recovery trigger
- PIT Channel 0 ISR – 4 Hz LED blinking

---

## Concurrency Design

Binary semaphores enforce strict execution order:

- Timer callback releases Task1
- Task1 releases Task2
- SW2 ISR releases Task3
- SW3 ISR restores normal scheduling

Task3 is assigned higher priority to guarantee emergency preemption.

Interrupt priorities are configured to respect:

```
configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
```

ISR-safe APIs used:

- xTimerStopFromISR
- xTimerStartFromISR
- xSemaphoreGiveFromISR

This ensures safe interaction between interrupt context and the FreeRTOS scheduler.

---

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

---

## Timing Overview

| Component               | Timing Behavior              |
|-------------------------|-----------------------------|
| Software Timer         | 2 second periodic callback  |
| PIT Interrupt          | 4 Hz LED toggle             |
| Emergency Activation   | Immediate via ISR           |
| System Resume          | Immediate via ISR           |

The design avoids blocking delays inside ISRs and maintains proper separation between interrupt context and task context.

---

## Technical Concepts Demonstrated

- Preemptive RTOS scheduling
- Interrupt-driven system design
- Binary semaphore synchronization
- ISR-safe FreeRTOS API usage
- Hardware timer configuration (PIT)
- Priority-based task control
- Deterministic state transitions
- Embedded systems debugging

---

## Hardware Platform

- NXP microcontroller (MCUXpresso environment)
- Onboard RED and GREEN LEDs
- Pushbuttons SW2 and SW3
- External LED connected to PTC16 (alarm output)

---

## Build Environment

- MCUXpresso IDE
- FreeRTOS Kernel
- NXP SDK drivers for GPIO and PIT

This repository includes application source and configuration files. Build artifacts and auto-generated IDE files are intentionally excluded.

---

## Purpose

This project demonstrates the implementation of a multitasking real-time embedded control system with strict state transitions and interrupt-driven emergency management. It highlights practical firmware engineering skills in RTOS design, synchronization, and hardware-level integration.