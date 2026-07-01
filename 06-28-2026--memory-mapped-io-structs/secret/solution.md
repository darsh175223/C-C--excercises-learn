# Solution — memory-mapped-io-structs (QTimer-16)

## Root cause

The `QTimer_CTRL` bitfield struct declares the fields **in the correct order
with the correct widths**, but uses the wrong **bitfield container type**:

```c
typedef struct {
    volatile uint16_t EN       : 1;   // unit #0, bit 0
    volatile uint16_t START    : 1;   // unit #0, bit 1
    volatile uint16_t MODE     : 2;   // unit #0, bits 2..3
    volatile uint16_t PRESCALE : 4;   // unit #0, bits 4..7
    volatile uint16_t RELOAD   : 16;  // <-- needs 16 bits, only 8 left in unit #0
} QTimer_CTRL;
```

A C bitfield is allocated within a storage unit of its declared type. With
`uint16_t`, the first four fields consume bits 0..7 of a **16-bit** unit, leaving
only 8 bits. A `:16` field cannot fit in those 8 remaining bits, so the compiler
(gcc, little-endian) **skips to the next 16-bit unit** and places `RELOAD` at
**bits [31:16]** — not the datasheet's **[23:8]**.

So the driver writes the right RELOAD value, but it lands 8 bits too high. The
hardware model reads bits [23:8] and gets garbage (the low byte of RELOAD shifted
up by 8, with the datasheet "reserved" bits [31:24] now non-zero). The timer
"runs" and the prescaler (bits [4:7], still in unit #0, correct) works — only the
period is wrong. That's exactly the reported symptom.

Note why it's hard to see:
- The field **order and widths match the datasheet exactly**, so a line-by-line
  read of the struct looks right.
- `sizeof(QTimer_CTRL)` is still **4 bytes** (two 16-bit units), so the
  `_Static_assert`s pass and COUNT/STATUS offsets are unaffected — no size clue.
- `-Wall -Wextra` says nothing; bitfield straddling/skipping is not diagnosed.
- Only inspecting the **actual bit positions at runtime** (dump the raw CTRL
  word) reveals RELOAD at [31:16].

## Minimal fix

Change the bitfield storage unit from `uint16_t` to `uint32_t` so all fields
share one 32-bit unit and `RELOAD:16` fits at bits [23:8]:

```diff
 typedef struct {
-    volatile uint16_t EN       : 1;
-    volatile uint16_t START    : 1;
-    volatile uint16_t MODE     : 2;
-    volatile uint16_t PRESCALE : 4;
-    volatile uint16_t RELOAD   : 16;
+    volatile uint32_t EN       : 1;
+    volatile uint32_t START    : 1;
+    volatile uint32_t MODE     : 2;
+    volatile uint32_t PRESCALE : 4;
+    volatile uint32_t RELOAD   : 16;
 } QTimer_CTRL;
```

`sizeof` stays 4, all three self-test checks pass.

(An equally valid alternative: keep `uint32_t` and split `RELOAD` into
`RELOAD_LO:8` / `RELOAD_HI:8` — but the container-type fix is the minimal one.
Hand-masking in `qtimer.c` is **not** a valid fix: it leaves the raw bits wrong
and `test_ctrl_layout` still fails.)

## Lesson

For memory-mapped registers, the bitfield container type is part of the layout,
not a cosmetic choice. A wide field must fit in the remaining space of its
current storage unit or the compiler silently relocates it to the next unit.
Always use a container at least as wide as the register (here `uint32_t`), and
verify register overlays by dumping raw words against the datasheet, never by
eyeballing the struct.
