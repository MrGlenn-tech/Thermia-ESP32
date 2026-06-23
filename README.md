# ThermIQ ESPHome MQTT compatibility firmware

ESPHome-port av Thermia/ThermIQ ESP32-firmware

## Vad denna firmware gör

- Flashas första gången via ESPHome Web eller ESPHome i Home Assistant.
- Uppdateras därefter via ESPHome OTA.
- Loggar visas via ESPHome/Home Assistant.
- Behåller I2C-slavlogiken på adress `0x2E`.
- Behåller MQTT-flödet från originalsketchen.


## Home Assistant / ESPHome

1. Kopiera `thermiq-esp32.yaml` till ESPHome i Home Assistant.
2. Kopiera värdena från `secrets-example.yaml` till din ESPHome `secrets.yaml`.
3. Validera/kompilera.
4. Första flash via USB/ESPHome Web.
5. Efter första ESPHome-flash kan du uppdatera OTA.

## Viktigt

Detta är avsett att vara MQTT-kompatibelt med originalsketchen. Använd inte ESPHomes inbyggda `mqtt:` samtidigt för samma topics, eftersom denna komponent själv kör `PubSubClient` och publicerar discovery/state/data.

## Kända saker att testa

- Att MQTT ansluter och publicerar `thermia/esp32/availability = online`.
- Att `ThermIQ/ThermIQ-mqtt/data` har samma fält som tidigare.
- Att HACS-integrationen reagerar på `d050`, `d051`, `EVU` och `INDR_T`.
- Att skrivning till r32/r33 fungerar mot pumpen.

# Anslut ESP 
GND to GND 
SCL GPIO22 (GPIO22 Kan inte vara ansluten vid uppstart av ESP låt den boota upp sedan anslut SCL till GPIO22 Jobbar på lösnig)
SDA GPIO21

<img width="321" height="157" alt="image" src="https://github.com/user-attachments/assets/08c0ad42-55d1-409c-b1c5-192abfd72b45" />

