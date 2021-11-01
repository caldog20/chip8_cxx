# CHIP-8 Information

- CPU
    - 16 8-bit registers
        - V0 - VF
        - VF is a flag register
    - 16-bit Index register
    - 16-bit Program Counter
    - 16-level stack
    - 8-bit Stack 



### Memory Layout
| Memory Range | Purpose|
|--------------|--------|
|0x000 to 0xFFF| Entire Range|
|0x050 to 0x0A0| Characters|
|0x200 to 0xFFF|Instruction space|



- Timers
    - Delay Timer 8-bit
    - Sound Timer 8-bit

- 16 Input Keys
    - 0 to F

- Display
    - 64x32