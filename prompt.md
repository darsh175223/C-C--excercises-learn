# Embedded C/C++ Learning Prompt

> **How to use this file.** From any coding agent (opencode, Claude, GPT, etc.) running inside this parent directory, say one of:
>
> - `Read prompt.md and generate a project for the topic "C++ Multithreading"`
> - `Read prompt.md and generate a random project`
> - `I'm done with 06-28-2026--cpp-multithreading, grade it`
>
> The agent reads this file in full, then executes the matching workflow below. Do not deviate from the rules here.

---

## 0. Global Rules (always apply)

1. **Never spoon-feed.** If the task is a bug fix, never say exactly what the bug is, where it is, or how to fix it. Phrase it as "there is a bug in `<file>` / `<function>` — investigate and fix." The grader workflow enforces a **3-strike rule** before any solution is revealed.
2. **`secret/` is off-limits to the learner.** Every generated project has a `secret/` folder containing the reference solution, the simulation harness, and the grading rubric. The learner is explicitly told **not** to read `secret/`. The agent must never print, cat, diff, or summarize `secret/` contents to the learner — not during generation, not during grading (except on strike 3 when the solution is finally revealed, and even then only the `solution.md` portion, not the harness internals if avoidable).
3. **Creative & non-trivial.** Projects must not be toy "blink an LED" hello-worlds. Each project embeds a realistic embedded-style bug (data race, off-by-one, priority inversion, volatile misuse, ISR starvation, etc.) OR a meaty feature to implement (lock-free ring buffer, bootloader swap, DMA descriptor chain). Avoid bugs that lint or the compiler would already catch — the learner should need a debugger, simulation output, or reasoning to find them.
4. **Runnable without physical hardware.** Every embedded project must be executable on the host using one of the approved simulators below. Provide exact run commands in `BUILD.md`. If a real device is needed by the *topic* (e.g. a sensor), simulate it inside `secret/` so the learner only interacts with code + simulator output.
5. **Same context for generation and grading.** The grader uses the exact same `prompt.md`, the project's `PROBLEM.md`, and `secret/rubric.md` + `secret/solution.md`. It does not "forget" how the project was built.
6. **Naming convention.** All project subdirectories use `MM-DD-YYYY--<topic-slug>`. Slug is lowercase, hyphenated, matches the topic list slug exactly (see §1). Example: `06-28-2026--cpp-multithreading`.
7. **Date.** Use the current date when the generate command is issued. If two projects share a date, append `-2`, `-3` etc. to the slug.

---

## 1. Topic Catalog (30 topics)

The agent must use these exact slugs. A "random" project picks uniformly from this list, skipping any topic the learner has already completed (detected by an existing `MM-DD-YYYY--<slug>` directory anywhere in the parent).

| #  | Slug                                | Domain       | Default Simulator     | Classic Bug / Feature Pattern                                  |
|----|-------------------------------------|--------------|-----------------------|----------------------------------------------------------------|
| 01 | memory-leaks-fragmentation          | C / host     | valgrind              | malloc/free imbalance + heap fragmentation simulator           |
| 02 | pointer-arithmetic-buf-overflow     | C / host     | ASan (`-fsanitize=address`) | off-by-one in packet parser, no compiler warning             |
| 03 | bitmasking-register-emulation       | C / host     | custom `secret/` peripheral sim | virtual MMIO register, wrong bit-field shift               |
| 04 | interrupt-driven-uart               | embedded     | Renode                | ISR ↔ main race on shared ring buffer                         |
| 05 | dma-transfer-sim                    | embedded     | QEMU (Cortex-M)       | DMA descriptor half-transfer callback bug                      |
| 06 | cpp-multithreading                  | C++ / host   | g++ + ThreadSanitizer | producer/consumer deadlock or missed notify                    |
| 07 | rtos-priority-inversion             | embedded     | Renode / picosim      | priority inversion w/o mutex inheritance                       |
| 08 | timers-pwm                           | embedded     | SimulAVR              | servo PWM duty cycle miscalculation (integer overflow)         |
| 09 | spi-master-slave                    | embedded     | Renode                | SPI framing: wrong CPOL/CPHA for slave                          |
| 10 | i2c-multi-master                    | embedded     | QEMU                  | arbitration loss mishandled, no stop bit                       |
| 11 | adc-sampling-filter                 | embedded     | custom `secret/` signal source | moving-average filter integer overflow                  |
| 12 | gpio-debouncing                     | embedded     | custom `secret/` noisy input | debounce FSM stuck state                                    |
| 13 | watchdog-timer                      | embedded     | Renode                | WDT timeout too long → system hangs before kick                |
| 14 | linker-scripts-sections             | embedded     | `arm-none-eabi-ld`    | symbol placed in wrong section, boot vector broken             |
| 15 | volatile-compiler-barriers          | C / host     | gcc `-O2`             | shared var optimized out of ISR poll loop                      |
| 16 | finite-state-machines               | C / host     | host exe              | missing transition causes stuck state                          |
| 17 | ring-buffer-lockfree                | C++ / host   | g++ + TSan            | SPSC queue missing acquire/release on tail index               |
| 18 | rtos-message-queue                  | embedded     | Renode                | queue overflow handled silently, data dropped                  |
| 19 | bootloader-dual-bank                | embedded     | QEMU                  | bank swap writes wrong magic word → brick (simulated)          |
| 20 | power-modes-sleep                   | embedded     | Renode                | sleep entered with peripheral clock ungated → brownout sim     |
| 21 | fault-recovery                       | embedded     | QEMU                  | HardFault handler clobbers stack, recovery fails               |
| 22 | fixed-point-arithmetic              | C / host     | host exe              | Q15 multiply overflow, no saturation                            |
| 23 | code-size-optimization              | embedded     | `arm-none-eabi-size`  | dead code + duplicate string literals bloat .rodata            |
| 24 | cross-compilation-toolchain         | embedded     | `avr-gcc` + SimulAVR  | wrong `-mmcu` flag, link script mismatch                       |
| 25 | hal-abstraction-layer               | C / host     | host exe              | HAL config struct wrong field order, init fails                |
| 26 | cpp-templates-constexpr-embedded    | C++ / host   | g++                   | constexpr register map wrong base offset                       |
| 27 | gdb-debug-symbols                   | C / host     | gdb                   | learner must write gdb script to catch bug; watchpoint target  |
| 28 | memory-mapped-io-structs            | C / host     | host exe + `secret/` MMIO shim | volatile bitfield struct packing wrong            |
| 29 | event-driven-callbacks              | C / host     | host exe              | observer reentrancy → use-after-free                           |
| 30 | unit-testing-unity-ceedling         | C / host     | Ceedling              | test coverage missing edge case; bug hides in untested path    |

**Approved simulator quick-reference** (agent must verify availability with `--version` before using; if missing, fall back to a host-C `secret/` shim that emulates the peripheral):

| Tool        | Install hint                          | Used for                |
|-------------|---------------------------------------|-------------------------|
| Renode      | `pip install renode-run` / `.deb`     | multi-arch MCU sim      |
| QEMU        | `qemu-system-arm`, `qemu-system-avr`  | Cortex-M, AVR           |
| SimulAVR    | `simulavr` package                    | AVR                     |
| picosim     | `pico-sdk` + `qemu-system-riscv32`    | RP2040 / RP2350         |
| PlatformIO  | `pip install platformio`              | unified build + simavr  |
| valgrind    | system package                        | host C leak detection   |
| ASan / TSan | built into gcc/clang                  | host memory/data race   |
| Ceedling    | `gem install ceedling`                | Unity test framework    |
| gdb         | system package                        | debugging               |

On Windows host (this machine): prefer WSL for valgrind/Renode/QEMU; if WSL unavailable, prefer host-C `secret/` shims + MSYS2 gcc/clang with sanitizers.

---

## 2. GENERATE Workflow

Trigger phrases:
- `generate a project for the topic "<slug>"` (slug must match §1)
- `generate a project for the topic "<display name>"` → agent maps display name → slug
- `generate a random project` → agent picks slug from §1, skipping completed

Steps the agent performs, in order:

### 2.1 Resolve target
1. Determine slug. Validate against §1 list. If unknown, list valid slugs and stop.
2. Compute today's date → folder name `MM-DD-YYYY--<slug>`.
3. Check parent directory: if folder exists, abort with message asking learner to pick a different topic or rename.

### 2.2 Scaffold the folder
Create:

```
<MM-DD-YYYY--<slug>>/
├── PROBLEM.md          # learner-visible: what to fix/build, what files do, run commands
├── BUILD.md            # learner-visible: exact commands to build + run + simulate
├── src/                # learner-visible source (or generated project tree)
├── (embedded: PlatformIO/Makefile/CMake project as appropriate)
├── tests/              # learner-visible tests (may be partial — learner may need to add)
└── secret/
    ├── README.md       # "DO NOT READ — grading only"
    ├── rubric.md       # pass/fail checklist for the grader
    ├── solution.md     # full explanation + reference patch
    ├── harness/        # simulator config / Renode .resc / QEMU script / MMIO shim source
    └── reference/      # reference implementation (compiles, passes tests)
```

### 2.3 Generate project per topic
- For **embedded** topics: run the real project-init command (e.g. `platformio init -b uno`, `mkdir` + hand-written `Makefile` for `arm-none-eabi-gcc`, Ceedling `new`, etc.). Do not fake it — actually invoke the tool so the learner has a real build system.
- For **host C/C++** topics: create `src/`, `tests/`, `Makefile` or `CMakeLists.txt`, and a `secret/` shim.
- Write `src/` with the **buggy** (or feature-incomplete) version of the code. Bugs must be real, subtle, and reproduce under the simulator — not crash-the-compiler obvious.
- Write `secret/reference/` with the **correct** version.
- Write `secret/harness/` with whatever simulator config is needed (Renode `.resc`, QEMU launch script, MMIO shim `.c`, signal generator for ADC topic, etc.).
- Write `secret/rubric.md`:
  - List of test commands to run (e.g. `make test`, `./build/sim --self-test`, Renode script with asserts).
  - Expected output / pass criteria (exact strings or regex).
  - Common partial-credit checkpoints (only used on strike 1/2 feedback).
- Write `secret/solution.md`:
  - Root cause explanation.
  - Minimal diff / patch to fix.
  - Why the bug is subtle (so the grader can echo this on strike 3).

### 2.4 Write PROBLEM.md (learner-visible)
PROBLEM.md must contain:
- **Topic** + short framing (2-3 sentences of realistic embedded context — not "this is exercise 7").
- **What each file in `src/` currently does** — one bullet per file.
- **Your task** — either:
  - *Bug-fix mode:* "There is a bug in `<file>` / `<function>`. Symptoms: `<observable behavior from simulation>`. Investigate and fix. Do not ask me to point you at it."
  - *Feature mode:* "Implement `<feature>` so that `<observable requirement>`. Constraints: `<list>`."
- **What NOT to read:** explicit warning that `secret/` exists for grading only and reading it voids the exercise.
- **Allowed vs forbidden changes** (e.g. "you may edit only `src/uart.c`", "do not modify `tests/`").
- **Pointer to BUILD.md** for run commands.

### 2.5 Write BUILD.md (learner-visible)
Exact, copy-pasteable commands. Include:
1. Prereqs check (e.g. `renode --version`).
2. Build command.
3. Run/simulate command — for embedded, the full simulator invocation (e.g. `renode sim/harness.resc`).
4. How to interpret pass/fail output (e.g. "you should see `ALL TESTS PASSED`; any `ASSERT FAIL` line means still broken").
5. Optional debug command (e.g. `gdb` attach to QEMU).

### 2.6 Verify before handing off
The agent MUST:
1. Build both `src/` (buggy) and `secret/reference/` — confirm both compile.
2. Run the harness against `src/` → confirm it **fails** (proves the bug reproduces).
3. Run the harness against `secret/reference/` → confirm it **passes** (proves the rubric is satisfiable).
4. If either check fails, fix the project before reporting done.

Report to learner: folder name, topic, where to read (`PROBLEM.md`), first command to run.

---

## 3. GRADE Workflow

Trigger phrases:
- `I'm done with <MM-DD-YYYY--<slug>>, grade it`
- `grade <slug>`
- `I'm done with <slug>, can you grade it?`

Steps:

### 3.1 Load context
1. Locate the project folder by slug (or full folder name).
2. Read `PROBLEM.md`, `BUILD.md`, `secret/rubric.md`, `secret/solution.md`. Do NOT display `secret/` contents to the learner.
3. Determine current strike count. Store strike count in `secret/.strikes` (a one-line file containing an integer, default 0 if absent). Increment on each grading attempt.

### 3.2 Run checks
1. Run every command listed in `secret/rubric.md` against the learner's current `src/`.
2. Capture pass/fail per check. Also run a diff/compile sanity (does it still build?).
3. Compare learner's edits against the allowed-changes rule from `PROBLEM.md` — if learner edited forbidden files, that's an automatic fail with a specific message ("you modified `tests/` which is forbidden — revert").

### 3.3 Decide outcome

- **All rubric checks pass** → learner wins. Print a short "PASS" message naming which checks passed, one-line reinforcement of the concept learned, and suggest the next topic. Zero out `secret/.strikes`. Do not reveal `solution.md` content (they already solved it).

- **Any check fails AND strike count < 3**:
  - Strike 1: report *which* check(s) failed and *the observable symptom* only. Do NOT name the file/function if possible; if the failing check is function-specific, name the check but not the root cause. Example: "`test_uart_rx_loop` failed: buffer underflow observed at 9600 baud. Keep digging."
  - Strike 2: add one *narrow* hint pointing at the symptom category (e.g. "the issue is a synchronization problem, not a buffer-size problem") — still no line numbers, no code, no fix.
  - Increment `secret/.strikes`.

- **Any check fails AND strike count == 3**:
  - This is the third failed attempt. Now reveal the solution.
  - Print `secret/solution.md` content in full: root cause, minimal patch, why subtle.
  - Apply the reference patch to a *copy* (`src.solved/`) so the learner can diff `src/` vs `src.solved/` and study. Do not overwrite learner's `src/`.
  - Re-run rubric against `src.solved/` to prove it passes.
  - Mark the topic complete (write `secret/.completed`).
  - Reset strikes.

### 3.4 Constraints during grading
- Never print `secret/harness/` source, `secret/reference/` source, or `secret/rubric.md` raw contents to the learner. Only the grader uses them.
- Never reveal the solution before strike 3, even if the learner begs or asks "just tell me the file."
- If the learner asks for hints outside of a grade request, you may give *topic-level* conceptual hints (e.g. "review what `volatile` guarantees vs. what a memory barrier guarantees") but not project-specific answers.

---

## 4. Example Session

```
user: read prompt.md and generate a project for the topic "cpp-multithreading"
agent: [creates 06-28-2026--cpp-multithreading/, runs platformio/make, verifies bug reproduces, verifies reference passes]
       Created `06-28-2026--cpp-multithreading`. Read `PROBLEM.md` first.
       Start: `cd 06-28-2026--cpp-multithreading && make run`

user: [edits src/queue.cpp]
user: I'm done with 06-28-2026--cpp-multithreading, grade it
agent: [runs rubric, 2/3 checks fail]
       Strike 1. `test_producer_consumer` deadlock after 4000 ops. `test_overflow_behavior` wrong.
       Symptom: consumer stalls. Investigate.

user: [edits again]
user: grade it
agent: [runs rubric, still failing]
       Strike 2. Hint: this is a memory-ordering / notification problem, not a buffer-size problem.

user: [edits again]
user: grade it
agent: [runs rubric, still failing]
       Strike 3. Revealing solution. [prints solution.md, creates src.solved/, proves it passes]
```

---

## 5. Quality Bar (agent self-check before declaring "done" on generation)

- [ ] Bug reproduces deterministically under the harness in `src/`.
- [ ] Reference solution passes the same harness.
- [ ] No compiler/`-Wall -Wextra` warning reveals the bug.
- [ ] `PROBLEM.md` does not name the bug or its location.
- [ ] `BUILD.md` commands are copy-paste runnable on this host.
- [ ] `secret/` contents are not leaked anywhere learner-visible.
- [ ] Project is non-trivial — a competent junior SWE would need ≥20 min and a debugger to solve.

If any box unchecked, regenerate or fix before reporting.
