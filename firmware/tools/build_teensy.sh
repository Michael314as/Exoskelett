#!/usr/bin/env bash
# Baut die Firmware für den Teensy 4.1 ohne Arduino-IDE und ohne PlatformIO, nur mit arm-none-eabi-gcc.
# Ergebnis: build/teensy/exo.hex (mit dem Teensy Loader oder `teensy_loader_cli --mcu=TEENSY41 -w exo.hex` flashen).
#
# Voraussetzungen (Debian/Ubuntu):  apt install gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
# Quellen (einmalig, Ordner als Argument übergeben):
#   git clone https://github.com/PaulStoffregen/cores        (Teensy-Core)
#   git clone https://github.com/tonton81/FlexCAN_T4          git clone https://github.com/PaulStoffregen/Wire
#   git clone https://github.com/PaulStoffregen/SPI            git clone https://github.com/PaulStoffregen/EEPROM
#   git clone https://github.com/adafruit/Adafruit_NeoPixel    git clone https://github.com/adafruit/Adafruit_BNO08x
#   git clone https://github.com/adafruit/Adafruit_BusIO       git clone https://github.com/adafruit/Adafruit_Sensor
# Aufruf:  tools/build_teensy.sh <pfad/zu/cores> <ordner/mit/bibliotheken>
set -euo pipefail
CORE="$(cd "$1/teensy4" && pwd)"; LIBS="$(cd "$2" && pwd)"
HERE="$(cd "$(dirname "$0")/.." && pwd)"; OUT="$HERE/build/teensy"; mkdir -p "$OUT/obj"
CPU="-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16"
DEFS="-D__IMXRT1062__ -DARDUINO_TEENSY41 -DTEENSYDUINO=159 -DARDUINO=10813 -DF_CPU=600000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH"
INC="-I$CORE -I$HERE/src"
for l in FlexCAN_T4 Wire SPI EEPROM Adafruit_NeoPixel Adafruit_BNO08x/src Adafruit_BusIO Adafruit_Sensor; do INC="$INC -I$LIBS/$l"; done
COMMON="$CPU -O2 -g -ffunction-sections -fdata-sections -nostdlib -MMD $DEFS $INC"
CFLAGS="$COMMON"
CXXFLAGS="$COMMON -std=gnu++17 -fno-exceptions -fpermissive -fno-rtti -fno-threadsafe-statics -felide-constructors -Wno-error=narrowing"

SRCS=$(ls "$CORE"/*.c "$CORE"/*.cpp)
SRCS="$SRCS $LIBS/Wire/Wire.cpp $LIBS/Wire/WireIMXRT.cpp $LIBS/SPI/SPI.cpp"
SRCS="$SRCS $(ls "$LIBS"/Adafruit_NeoPixel/*.c "$LIBS"/Adafruit_NeoPixel/*.cpp 2>/dev/null)"
SRCS="$SRCS $(ls "$LIBS"/Adafruit_BNO08x/src/*.c "$LIBS"/Adafruit_BNO08x/src/*.cpp)"
SRCS="$SRCS $(ls "$LIBS"/Adafruit_BusIO/*.cpp) $LIBS/Adafruit_Sensor/Adafruit_Sensor.cpp"
SRCS="$SRCS $HERE/src/main_teensy.cpp"

OBJS=""
for f in $SRCS; do
  o="$OUT/obj/$(echo "$f" | md5sum | cut -c1-8)_$(basename "$f").o"
  case "$f" in
    *.c) arm-none-eabi-gcc $CFLAGS -c "$f" -o "$o" ;;
    *.cpp) arm-none-eabi-g++ $CXXFLAGS -c "$f" -o "$o" ;;
  esac
  OBJS="$OBJS $o"
done
# eigene Dateien zusätzlich mit allen Warnungen
arm-none-eabi-g++ $CXXFLAGS -Wall -Wextra -c "$HERE/src/main_teensy.cpp" -o "$OUT/obj/main_check.o"

arm-none-eabi-gcc $CPU -O2 -Wl,--gc-sections,--relax -T"$CORE/imxrt1062_t41.ld" -o "$OUT/exo.elf" $OBJS \
  -L"$CORE" -lm -lstdc++
arm-none-eabi-objcopy -O ihex -R .eeprom "$OUT/exo.elf" "$OUT/exo.hex"
arm-none-eabi-size "$OUT/exo.elf"
echo "Fertig: $OUT/exo.hex"
