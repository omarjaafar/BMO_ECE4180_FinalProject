# ECE 4180 Final Project  
## BMO Tamagotchi Companion

<img width="1861" height="1126" alt="BMO_Circuit_Diagram drawio" src="https://github.com/user-attachments/assets/3fa4689a-d563-421d-a807-93e78ca8ebbd" />


For our final project, we created an interactive BMO inspired Tamagotchi system using an ESP32 C6 microcontroller and a secondary ESP32 board with an IMU sensor. Our goal was to build a small digital companion that shows emotions, plays games, reacts to the user, and manages virtual stats like happiness and energy. The project combines a display, audio feedback, user input, wireless communication, a microphone, and low power features into one system.

## Project Overview

The main ESP32 C6 controls a 4D Systems uLCD 144 G2 screen which draws the BMO face, menus, stat bars, and our Snake game. A 5 way navigation switch allows the user to move through the menus and control games. A common anode RGB LED displays BMO's emotional state using different colors. A piezo speaker plays sound effects such as menu tones, game over sounds, and a small melody. A microphone module provides basic voice or noise detection that triggers BMO to animate and respond.

A secondary ESP32 board with an IMU acts as a wireless accessory. When you shake the IMU, it sends a happiness boost to the main BMO device over ESP NOW. This gives the project a fun physical interaction and demonstrates wireless communication between embedded devices.

We also implemented happiness and energy stat decay, a real time clock that syncs through NTP, a low power mode, and a complete Snake game. The overall interaction is similar to a Tamagotchi where a virtual pet needs attention, but ours includes more modern features like animations, sound, wireless events, and extra interactivity.

## Components and Their Roles

### ESP32 C6 (Main Controller)
This is the main processor. It handles the UI, stat system, audio, navigation switch, microphone input, ESP NOW receiving, and low power mode. We chose it because it supports WiFi, hardware PWM, multiple GPIOs, and has enough performance for graphics and sound at the same time.

### 4D Systems uLCD 144 G2
We wanted a colorful and expressive display for BMO's face, menus, and games. The Goldelox display communicates over UART and is much more flexible than simple character LCDs, which helps bring the character to life.

### 5 Way Navigation Switch
This is the main input device. It provides up, down, left, right, and center clicks which are used for navigation and gameplay.

### RGB LED (Common Anode)
The LED color reflects BMO's emotional state. For example, green or yellow indicates happiness and blue indicates low happiness. This adds personality and quick visual feedback.

### Piezo Speaker
We used the ESP32 LEDC PWM hardware to generate tones for menu movement, eating food in Snake, game over, and a short melody. Audio helps make interactions feel more responsive.

### Microphone Module
The microphone allows BMO to react to speech. When it detects speech, BMO is able to decipher the speech with the OpenAI Whisper API and respond. This makes the system feel more alive.

### Secondary ESP32 With IMU
This board detects shaking and sends that event to the main ESP32 using ESP NOW. It works like an external toy that increases BMO's happiness when shaken. This also demonstrates a real wireless sensor node design.

### Battery Pack and Power Board
These provide portable power to the system and step the voltage to a stable level.

## Problems Encountered

We ran into several issues during development. The display sometimes needed exact reset timing, especially after waking from sleep. The navigation switch produced bouncing, so we had to use latched input handling. The microphone was sensitive and gave inconsistent triggers, which made the chatbot reaction feature a bit unpredictable. ESP NOW had to be reinitialized after waking from low power mode. Finally, we ran out of time to fully integrate everything into our 3D printed BMO enclosure.

## Comparison to Real Embedded Systems

Our system is similar to real embedded devices because it uses multiple peripherals together. It relies on non blocking timing, state machines, wireless communication, low power operation, and user interface design. These are all common in consumer electronics. It is different from a commercial Tamagotchi because ours has a color screen, sound effects, a wireless IMU companion, animations, and a game engine.

## Future Improvements

If we had more time or resources, we would refine several parts of the system. We would fully integrate the navigation switch, microphone, and display inside the 3D printed BMO housing. We would add more minigames. We would replace or amplify the microphone to make the chatbot feature more reliable. We would refine the Snake game by preventing wrap around movement and adding difficulty levels. We would also improve power optimization and make the UI animations smoother.

## Conclusion

Overall, our BMO Tamagotchi companion was a successful and enjoyable project. It brought together many topics from ECE 4180 including displays, wireless communication, sensors, PWM audio, low power modes, and embedded software structure. Although there are many ways we could refine or expand it, the final system works well and feels like a playful digital character.
