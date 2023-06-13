# Projekt ASB LAB 2023

Jan Bylicki 145441
Patryk Kościk 144635

## Motywacja projektu

Projekt ma na celu wykyrywanie oraz alertowanie stacji bazowej o upadkach osób starszych.

Wykorzystane zostały następujące technologie:
* Zephyr RTOS: Najpopularniejszy RTOS obecnie na rynku
* TensorflowLite Micro: Minimalna implementacja TF pozwalajaca uruchamiać modele na systemach wbudowanych
* Bluetooth Low Energy: Atrybuty GATT umożliwiają nisko-prądową, asynchroniczną komunikację z peryferiami
* Renode: Deterministyczne testowanie aplikacji systemu wbudowanego
* Python: Cross-platform język wysokiego poziomu, wykorzystuje bibliotekę `BLEAK` do komunikacji z urządzeniami BLE
* GIT: Kontrola wersji w zespole



* Wytrenowanie sieci zajeło około godzine, 45 minut na zebranie datasetu, reszta czasu była spędzona na faktycznym trenowaniu modelu.
* Mieliśmy duże problemy z uruchomieniem wbudowanego w Arduino NANO 33BLE akcelerometru, oraz CDC 30 godzin.
** CDC: Modyfikacja DeviceTree oraz PRJ.conf
** Akcelerometr: Modyfikacja DeviceTree oraz PRJ.conf
* Konfiguracja BLE zajeła bardzo dużo czasu oraz objętości kodu. Łącznie poswięcono na ten temat około 10 godzin.
** Nauka BLE
** Projekt serwisu oraz charaktersytyk
** Implementacja kodu do istniejącego codebase
** Napisanie aplikacji sieciowej

Łączny poswięcony czas na projekt: około 40-45 godzin

Projekt spełnia wszystkie wymagania projektowe (funcjonalne oraz pozafunkcjonalne).



## Aplikacja kliencka:

Wymagania: python3, bleak
`pip3 install bleak`
`python3 host/discovery.`


## Aplikacja Zephyrowa:

`pip3 install tensorflow tflite`
Wymagania: Zephyr >= 3.1, Zephyr-SDK, tensorflow
Kompilacja: 
- Umieścić folder z repozytorium w głównym katalogu Zephyra
- `west build -b arduino_nano_33_ble --pristine`
- `west -v flash --bossac=./bossac/bossac`

Bossac zamieszczony w repozytorium został skompilowany z forka arduino tak, aby obsłużyć bootloader zamieszczony na płytce.


## Struktura projektu

```
.
├── app.overlay
├── bossac
│   └── bossac        # Arduino bootloader flasher
├── CMakeLists.txt
├── deploy_ble.sh     # Deploy script
├── prj.conf          # Zephyr prj config
├── README.md
├── renode            # Renode simulation infra
│   └── machine.resc
├── host              # Desktop/RPI application 
│   └── discovery.py 
├── src
│   ├── accelerometer_handler.cpp
│   ├── accelerometer_handler.hpp
│   ├── assert.cpp
│   ├── constants.hpp
│   ├── gatt_write_common.c           # Common files for GATT char. support
│   ├── gatt_write_common.h
│   ├── magic_wand_model_data.cpp     # TF Lite Model data
│   ├── magic_wand_model_data.hpp
│   ├── main.cpp                      # Main loops calls
│   ├── main_functions.cpp            # Main loops definitions
│   ├── main_functions.hpp
│   ├── peripheral_gatt_write.c       # GATT communication logic
│   └── peripheral_gatt_write.h
└── train                             # Follow README.md to recreate training (dataset is already provided)
    ├── data
    ├── data_augmentation.py
    ├── data_augmentation_test.py
    ├── data_load.py
    ├── data_load_test.py
    ├── data_prepare.py
    ├── data_prepare_test.py
    ├── data_split_person.py
    ├── data_split_person_test.py
    ├── data_split.py
    ├── data_split_test.py
    ├── model_quantized.tflite
    ├── model.tflite
    ├── negative
    ├── README.md
    ├── train_magic_wand_model.ipynb
    ├── train.py
    ├── train_test.py
    └── wing
```





