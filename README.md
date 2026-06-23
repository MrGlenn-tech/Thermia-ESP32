# ThermIQ ESPHome MQTT compatibility firmware

ESPHome-port av Thermia/ThermIQ ESP32-firmware med bibehållna MQTT-topics och payload-format för befintlig HACS-integration.

## Vad denna firmware gör

- Flashas första gången via ESPHome Web eller ESPHome i Home Assistant.
- Uppdateras därefter via ESPHome OTA.
- Loggar visas via ESPHome/Home Assistant.
- Behåller I2C-slavlogiken på adress `0x2E`.
- Behåller MQTT-flödet från originalsketchen.

## MQTT-topics som behålls

Publicerar:

```text
thermia/esp32/state
thermia/esp32/availability
ThermIQ/ThermIQ-mqtt/data
homeassistant/sensor/thermia_esp32/<key>/config
```

Prenumererar:

```text
thermia/esp32/set
ThermIQ/ThermIQ-mqtt/set
ThermIQ/ThermIQ-mqtt/write
```

`base_topic`, `thermiq_base_topic`, `discovery_prefix`, `device_id` och `device_name` kan ändras i YAML, men defaults matchar originalkoden.

## Filer

```text
thermiq-esp32.yaml
secrets-example.yaml
components/thermiq_esp32/__init__.py
components/thermiq_esp32/thermiq_esp32.h
components/thermiq_esp32/thermiq_esp32.cpp
```

## GitHub

1. Skapa ett repo, till exempel `thermiq-esphome`.
2. Ladda upp hela innehållet i denna mapp till repot.
3. Ändra i `thermiq-esp32.yaml`:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/DITT-GITHUB-NAMN/thermiq-esphome
      ref: main
    components: [thermiq_esp32]
```

till din riktiga GitHub-URL.

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
