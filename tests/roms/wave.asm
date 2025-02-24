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
    ld a, $04       ; Enable Wave to left only
    ld [rNR51], a
    ld hl, _AUD3WAVERAM     ; Fill Wave RAM with square wave
    ld a, $FF
    ld b, 8
FillWave:
    ld [hl+], a
    dec b
    jr nz, FillWave
    ld a, $00
    ld b, 8
FillWave2:
    ld [hl+], a
    dec b
    jr nz, FillWave2
    ld a, $80       ; Enable Wave
    ld [rNR30], a
    ld a, $20       ; Volume 100%
    ld [rNR32], a
    ld a, $D0       ; Frequency low
    ld [rNR33], a
    ld a, $87       ; Frequency high, trigger
    ld [rNR34], a
Loop:
    jr Loop