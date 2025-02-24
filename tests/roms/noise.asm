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
    ld a, $F0       ; Envelope: max volume, no fade
    ld [rNR42], a
    ld a, $40       ; Noise frequency (mid-range)
    ld [rNR43], a
    ld a, $80       ; Trigger
    ld [rNR44], a
Loop:
    jr Loop