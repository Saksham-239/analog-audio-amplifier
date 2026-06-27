# Bill of Materials (BOM)

This document lists all components required to build the hybrid digital-analog audio amplifier system, categorized by functional block.

## 1. Digital Control & Audio Source

| Item | Component | Qty | Reference / Specs | Package | Function |
| :--- | :--- | :---: | :--- | :--- | :--- |
| 1.1 | ESP32 Development Board | 1 | ESP32-WROOM-32E | DIP-38 / DevKitC | Microcontroller, MP3 decoding, sensor processing |
| 1.2 | MicroSD Card Module | 1 | SPI Interface, FAT32 support | Module | Houses the MP3/WAV files |
| 1.3 | I2S DAC Module | 1 | PCM5102A (recommended) | Module | High-fidelity digital-to-analog audio converter |
| 1.4 | HC-SR04 Proximity Sensor | 2 | Ultrasonic transceiver (4-pin) | Module | Gestural volume and mute/power control |
| 1.5 | Relay Module | 1 | 5V SPDT Relay with transistor driver | Module | Physical speaker mute / safety disconnect |

---

## 2. Active Tone Control Stage

| Item | Component | Qty | Reference / Specs | Package | Function |
| :--- | :--- | :---: | :--- | :--- | :--- |
| 2.1 | XH-M164 Tone Board | 1 | NE5532 Dual Low-Noise Op-Amp | Complete Module | Baxandall-style active Bass / Treble / Volume control |

*Note: If building from scratch, the tone board utilizes 2x NE5532 operational amplifiers, 100kΩ log potentiometers for controls, and a standard RC shelving network.*

---

## 3. Discrete Class-AB Power Amplifier Stage

### Active Semiconductors

| Item | Component | Qty | Description | Package | Function |
| :--- | :--- | :---: | :--- | :--- | :--- |
| 3.1 | BC109BP | 1 | NPN Small-Signal Transistor | TO-18 / TO-92 | Pre-driver / voltage amplification stage (VAS) |
| 3.2 | 2N3906 | 1 | PNP Small-Signal Transistor | TO-92 | $V_{BE}$ multiplier active bias regulator |
| 3.3 | 1N4148 | 2 | Fast Switching Diode | DO-35 | Bias string temperature compensation diodes |
| 3.4 | BD139 | 1 | NPN Medium-Power Transistor | TO-126 | Class-AB complementary push-pull upper output transistor |
| 3.5 | BD140 | 1 | PNP Medium-Power Transistor | TO-126 | Class-AB complementary push-pull lower output transistor |

### Passive Resistors (all 1/4W metal film, unless specified)

| Item | Component | Qty | Value | Package | Function |
| :--- | :--- | :---: | :--- | :--- | :--- |
| 3.6 | R1, R3 | 2 | 27 kΩ | Axial | Bias feedback resistors for input stage |
| 3.7 | R2 | 1 | 100 kΩ | Axial | Input impedance stabilizing resistor |
| 3.8 | R4 | 1 | 15 kΩ | Axial | Collector load resistor for VAS stage |
| 3.9 | R5 | 1 | 330 Ω | Axial | Pre-driver collector filtering resistor |
| 3.10 | R6 | 1 | 22 kΩ | Axial | AC negative feedback resistor |
| 3.11 | R7 | 1 | 1 kΩ | Axial | Feedback emitter resistor |
| 3.12 | PR1 | 1 | 470 Ω (Adjustable) | Trimmer Pot | Quiescent bias current ($I_q$) adjustment |
| 3.13 | R9, R10 | 2 | 10 Ω (1W or 2W) | Power Axial | Emitter stability and current-sharing resistors |
| 3.14 | RL | 1 | 8 Ω | Loudspeaker | Load speaker impedance |

### Passive Capacitors (electrolytic, rated 25V minimum)

| Item | Component | Qty | Value | Package | Function |
| :--- | :--- | :---: | :--- | :--- | :--- |
| 3.15 | C1 | 1 | 10 μF | Radial | Input DC-blocking coupling capacitor |
| 3.16 | C2 | 1 | 470 μF | Radial | Feedback AC-bypass capacitor |
| 3.17 | C3, C4 | 2 | 330 μF | Radial | Power supply line filter & bootstrapping capacitors |
| 3.18 | C5 | 1 | 2200 μF | Radial | Output DC-blocking capacitor (essential for single supply) |
| 3.19 | C6 | 1 | 2200 μF | Radial | Power rail bulk decoupling capacitor |

---

## 4. Power & Miscellaneous

| Item | Component | Qty | Reference / Specs | Package | Function |
| :--- | :--- | :---: | :--- | :--- | :--- |
| 4.1 | Dual DC Power Supply | 1 | $\pm12\text{V}$ to $\pm15\text{V}$ (for Tone Board) | Bench Supply | Front-end preamp power |
| 4.2 | Single DC Power Supply | 1 | $+15\text{V}$ (for Power Amplifier Stage) | Bench Supply | High-current power stage power |
| 4.3 | TO-126 Heatsinks | 2 | Clip-on / bolt-on heatsinks | Extruded | Thermal dissipation for BD139 & BD140 |
| 4.4 | Jumper Wires | - | Male-Male, Male-Female, Female-Female | - | Breadboard / Module interconnects |
| 4.5 | Breadboard | 2 | Standard 830-point breadboard | - | Prototyping workspace |
