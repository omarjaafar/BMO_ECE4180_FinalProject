## What's Left

- Battery power integration  
- Connect the IMU and add a “shake to replenish energy” feature  
- Add a push button with an interrupt  
- Refine pet state and preferences  
- Implement stat decay when the tomogatchi is turned off or ignored  
  - (Use NVS to save energy/happiness; NVS is already used for Snake high score)
- Add more settings (maybe an admin panel)
- Wi-Fi integration  
  - Possibly add chatbot functionality  
  - Or simpler: let the user connect to the ESP32 from a phone and feed BMO remotely

---

## ⚠️ Current Issue Summary

Right now, the ESP32-C6 is failing to boot into our program.  
It stays stuck on the default 4D Systems splash screen.  
This happens when the chip cannot enter normal boot mode.

---

## 🟥 Root Cause: GPIO0 Pulled LOW

GPIO0 is a **critical boot-strap pin** on the ESP32-C6.

If GPIO0 is LOW at reset, the ESP32:

- enters the wrong boot mode  
- never starts our program  
- stays in a “reset-like” state  
- leaves the LCD stuck on its startup splash  
- refuses uploads unless manually forced into bootloader mode  

I temporarily fixed this by connecting a resistor from **3.3V → GPIO0**, which forced the pin HIGH long enough for the board to boot once.  
After that, GPIO0 became unreliable again — likely due to an internal pull-up failure or damage.

---

## 🧪 What to Try Next

Test using the **other ESP32-C6** we have.

If the second board boots normally, the original ESP32’s GPIO0 is likely damaged or permanently stuck LOW.

---

## 🟩 Important: Do NOT Use GPIO0 for the RGB LED

On the new ESP32, **move the RGB LED off of GPIO0**.  
GPIO0 is extremely sensitive and directly affects the boot mode.  
Driving an LED on it can interfere with booting and was likely the cause of the instability.

---

## ✔️ Possible Fix for the Original Board

Add a **pull-up resistor** (1k–10k):

```text
3.3V → [ Resistor ] → GPIO0
