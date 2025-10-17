# 🖨️ UART “Console” – Largest Prime Finder (BlackBoard, C on ARM)

A personal embedded project that turns the BlackBoard’s USB-UART link into a tiny **interactive console**.  
The program prompts for an integer (≥ 2) over serial and returns the **largest prime ≤ N**.  
Implements raw **memory-mapped UART** (no stdio retargeting) and a simple integer parser with echo/backspace handling.

---

## ✨ Features
- Minimal **UART driver** (init, TX/RX, blocking I/O) via memory-mapped registers.
- Line input with digit filtering, **ENTER** to submit, and backspace support.
- Efficient prime number search (√N, odd divisors only).
- User feedback and error handling for invalid inputs.

---

## 🧰 Hardware / Memory Map
| Register | Address | Purpose |
|---|---|---|
| `UART1_CR` | `0xE0001000` | Control (reset, enable TX/RX) |
| `UART1_MR` | `0xE0001004` | Mode (8-N-1, no parity) |
| `UART1_BAUDGEN` | `0xE0001018` | Baud generator (115200) |
| `UART1_BAUDDIV` | `0xE0001034` | Baud divider (115200) |
| `UART1_SR` | `0xE000102C` | Status (TX FIFO full, RX empty) |
| `UART1_DATA` | `0xE0001030` | TX/RX FIFO data |

- Baud Rate: **115200**, 8-N-1, no flow control
- Constants: `BaudGen115200 = 0x7C`, `BaudDiv115200 = 6`

---

## 🧠 How It Works
1. **UART Initialization** – Reset, configure 115200 baud, enable TX/RX.
2. **Input Handling** – Read digits, support backspace, submit on Enter.
3. **Prime Search** – Decrement until the largest prime ≤ N is found.
4. **Output** – Send result back over UART with proper CRLF formatting.

### Example
```
Enter a positive integer (>= 2), then press ENTER:  9
Largest prime <= 9 is: 7
```

---

## 🧪 Build & Run Instructions
1. Open Vitis or XSCT environment.
2. Create a **Standalone C Application** targeting the BlackBoard BSP.
3. Add `uart_prime.c` (your `lab3.c`) to the `src` folder.
4. Build and **Run on Hardware**.
5. Open a serial terminal on your PC:
   - Port: BlackBoard USB-UART
   - Baud: 115200
   - Data: 8 bits
   - Parity: None
   - Stop bits: 1
6. Interact through the terminal.

---

## 📂 Suggested Repository Structure
```
uart-prime-console/
├─ README.md
├─ src/
│  └─ uart_prime.c
├─ Images/
│  └─ terminal_output.jpg
└─ Docs/
   └─ lab_report.pdf
```

---

## 🚀 Future Enhancements
- Non-blocking UART using interrupts or FIFO buffering.
- Extra commands (`help`, `bench`, etc.) for a richer console.
- Prime factorization mode.
- Improved line editing (arrow keys, input history).

---

## 🙋 Author
**Rabeea Fatima**  
Electrical & Computer Engineering  
Focus: Embedded Systems · FPGA · Microprocessors
