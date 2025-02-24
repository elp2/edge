include "hardware.inc"

section "Header", rom0[$100]
    nop
    jp Start

section "HeaderData", rom0[$104]
    ds $150 - $104  ; Placeholder for header

section "Main", rom0
Start:
    ld a, $80       ; Power on APU
    ld [rNR52], a
    ld a, $FF       ; Enable Square 1 to both outputs
    ld [rNR51], a

    ; Initial setup for Square 1
    ld a, $40       ; Duty 25% (01), length = 32 ticks (ignored)
    ld [rNR11], a
    ld a, $F8       ; Envelope: initial vol $F, decrease, step 8 (~125 ms/step)
    ld [rNR12], a
    ld a, $D0       ; Frequency low (~440 Hz)
    ld [rNR13], a
    ; Sweep off for simplicity (focus on envelope)
    ld a, $00       ; No sweep
    ld [rNR10], a

MainLoop:
    ; Trigger the channel to restart envelope
    ld a, $86       ; Frequency high $06, trigger (bit 7), no length control
    ld [rNR14], a

    ; Delay to let envelope complete (~2 seconds)
    ld bc, $8000    ; Rough delay loop (~2 seconds at 4.19 MHz)
Delay:
    dec bc
    ld a, b
    or c
    jr nz, Delay

    jr MainLoop     ; Repeat indefinitely