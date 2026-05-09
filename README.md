# PhantomChat

PhantomChat is a portable, completely offline, and anonymous messaging system built for ESP32 microcontrollers. It requires no internet, no cellular service, and no external infrastructure. 

By utilizing the ESP32 as a standalone Wi-Fi Access Point and a DNS-driven Captive Portal, PhantomChat creates an instant local chat room. Anyone who connects to the device's Wi-Fi network is automatically greeted with a modern, responsive web app directly on their screen—no app installation required.

| | | |
|:---:|:---:|:---:|
| <img width="269" height="568" alt="1 (Phone)" src="https://github.com/user-attachments/assets/01e2bbce-139a-4b54-a99f-0e80321be704" /> | <img width="269" height="568" alt="2 (Phone)" src="https://github.com/user-attachments/assets/e1bc0628-7db2-4162-9a92-a8bd861d631e" /> | <img width="269" height="568" alt="3 (Phone)" src="https://github.com/user-attachments/assets/2102c426-baf9-4fde-92b6-b0ae12d8a199" /> |


## Key Features

* **True Captive Portal:** Works seamlessly on iOS, Android, macOS, and Windows. Connecting to the network automatically opens the chat interface.
* **Modern Interface:** A highly polished, mobile-first design featuring message bubbles, auto-scrolling, and clean typography.
* **Anonymous & Color-Coded:** Users are assigned a unique, random ID upon connection. Bubbles are automatically color-coded using a hashing algorithm based on the user's ID to easily distinguish participants.
* **Custom Nicknames:** Users can choose to remain completely anonymous or set a custom display name.
* **Direct Messaging:** Includes a dynamic dropdown menu to send private messages. (Note: Handled client-side for rapid local deployment).
* **Interactive Reactions:** Tap any message to open a floating menu and react to specific messages in real-time.
* **Audio & Haptic Feedback:** Built-in toggle for notification sounds and device vibration when new messages arrive (utilizing the Web Audio API to bypass strict browser media policies).
* **Hardware LED Feedback:** Integrates with onboard WS2812B RGB LEDs to blink visually when users join or messages are routed.

## Advantages

* **Absolute Privacy:** There is no database, no central server, and no cloud storage. 
* **Zero Data Retention:** The system operates entirely in the ESP32's volatile RAM. Once the power is disconnected, all messages, user IDs, and metadata are permanently destroyed.
* **No Infrastructure Needed:** It operates completely off-grid. If you have a battery pack and the ESP32, you have a functioning communication network.
* **Cross-Platform Compatibility:** Because the interface relies on standard HTML, CSS, and JavaScript, it works on any device with a web browser.

## Real-World Applications

PhantomChat is designed for situations where traditional networks are unavailable, untrusted, or overloaded:

* **Off-Grid & Camping:** Communicate with your group around a large campsite or at a festival where cell towers are overloaded or nonexistent.
* **Airplanes & Cruises:** Devices can connect to the local ESP32 network while maintaining airplane mode, allowing families or colleagues to chat across the cabin without purchasing expensive satellite Wi-Fi.
* **Classrooms & Offices:** Pass messages silently in closed-room environments.
* **Conventions & Crowded Events:** Bypass congested cellular networks by maintaining a localized, private mesh for your team.
* **Disaster Recovery:** Instant pop-up communication in areas where infrastructure has been damaged by severe weather or power outages.

## Hardware Compatibility

The firmware is lightweight and designed to run on the ESP32 architecture. 

* **Tested Boards:** ESP32-WROOM, ESP32-C3 Zero, ESP32-S2, ESP32-S3.
* **LED Support:** Built-in support for WS2812B LEDs. (Configured to Pin 10 by default for ESP32-C3 Zero boards, easily adjustable in the code).
* **Power Requirements:** Can be run from a standard USB power bank or integrated with a 3.7V LiPo battery and a TP4056 charging module for a pocket-sized form factor.

## Flashing Instructions

To deploy PhantomChat to your ESP32, follow these steps using the Arduino IDE:

### 1. Prerequisites
* Install the latest Arduino IDE.
* Install the ESP32 Board Manager (by Espressif Systems).
* Open the Library Manager (Sketch > Include Library > Manage Libraries) and install the **Adafruit NeoPixel** library.

### 2. Board Configuration
* Select your specific ESP32 board from the Tools menu.
* **Crucial Step:** To prevent Wi-Fi caching issues, ensure that "Erase All Flash Before Sketch Upload" is set to ENABLED in the Tools menu for the initial flash.

### 3. Uploading
1. Open the provided `.ino` file in the Arduino IDE.
2. If using a board other than the ESP32-C3 Zero, check the `#define LED_PIN` variable and adjust it to match your board's built-in RGB LED pin.
3. Connect your ESP32 via USB and select the appropriate COM port.
4. Click Upload.

### 4. Post-Install Network Reset
If you are flashing over an older version of PhantomChat, ensure you tell your smartphone or computer to "Forget" the Wi-Fi network before reconnecting to clear the device's Captive Portal cache.

## Usage

1. Power the ESP32 via USB or battery.
2. Open the Wi-Fi settings on your phone, tablet, or laptop.
3. Connect to the network named **PhantomChat**.
4. The device should automatically prompt you to "Sign into network", which will open the chat interface. If it does not pop up automatically, open a web browser and navigate to `192.168.4.1`.
5. Tap the Sound button to enable audio notifications, set a nickname, and begin chatting.

## Disclaimer

This is a local, ad-hoc communication tool. Direct messages are filtered client-side for speed and simplicity. It is not intended for the transmission of highly sensitive or classified data against persistent, local network sniffers. Use responsibly.
