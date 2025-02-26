include "hardware.inc"

section "Header", rom0[$100]
    nop
    jp Start

section "HeaderData", rom0[$104]
    ds $150 - $104

section "Main", rom0
Start:
    ld a, $80       ; Power on APU
    ld [rNR52], a
    ld a, $80       ; Duty 50%
    ld [rNR21], a
    ld a, $F0       ; Envelope: max volume, no fade
    ld [rNR22], a
    ld a, $40       ; Frequency Low 8 bits
    ld [rNR23], a
    ld a, $C7       ; Trigger | Length Enable ; Frequency High
    ld [rNR24], a
Loop:
    jr Loop