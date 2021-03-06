# Xprize-ocean-sensor
## Project Description
This project addresses part of the sensing section of the Ocean XPRIZE: designing, building, and operating an ocean water sensing system comprised of: (1) an optical colorimeter sensor for measuring rhodamine dye; (2) a metal electrode electrochemical sensor to measure pH; (3) sensor electronic interfaces for each sensor; (4) wireless communication from the system to a computer.  The electronics and optics were designed independently and implemented using a soldered printed circuit board, and operated on a 3.7 V Lithium Ion battery.  The system communicated wirelessly with a computer using an IEEE Standard compliant communication system (WiFi ESP8266).  The system was immersed in ocean water for testing, and thus, the packaging was leak proof. Validation and testing with known and unknown water samples, system power consumption, and environmental impact assessment were performed.
## Main System Components
1. Feather Huzzah (w/ ESP8266)
2. 16-bit Adafruit ADC
3. Adafruit TCS34725 Color Sensor
4. Adafruit NeoPixel RGBW LED
5. 3.7v Lithium Ion battery
6. Soldered circuit boards
7. Titanium and Zinc rods
8. PVC piping (waterproofing)
9. TP-Link Nano Wireless Access Point
## Included Files
1. Xprize_final_report.pdf - the final report, which details background research, building and testing processes, results, discussion, and includes pictures
2. hardware_upload.ino - arduino code that was uploaded to the Feather Huzzah and ran all components of the project, including activating LEDs, reading sensor data, and establishing wireless communication
3. data_plotting_analytics.m - MATLAB code to process, plot, and analyze data from the ThingSpeak cloud once testing is complete
## Project Collaborators
- Pritak Patel, Duke University
- Ryan Anders, Duke University
- Sam Kelly, Duke University