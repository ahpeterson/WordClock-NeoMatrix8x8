#!/bin/bash
# Read or set the word clock's DS3231 RTC over USB (macOS).
#
# Usage: set_rtc.sh upload   upload the set_rtc utility sketch to the clock
#        set_rtc.sh read     print the RTC time for a few seconds
#        set_rtc.sh set      set the RTC to this Mac's local *standard* time
#
# Afterwards, re-upload WordClock_NeoMatrix8x8 so the clock runs normally again.
# Set PORT=/dev/cu.whatever to override the auto-detected serial port.
# Close the Arduino IDE's Serial Monitor first, or the port will be busy.

HERE="$(cd "$(dirname "$0")" && pwd)"

CLI="$(command -v arduino-cli)"
[ -z "$CLI" ] && CLI="/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli"
if [ ! -x "$CLI" ]; then echo "arduino-cli not found" >&2; exit 1; fi

PORT="${PORT:-$(ls /dev/cu.usbserial-* /dev/cu.usbmodem* 2>/dev/null | head -1)}"
if [ -z "$PORT" ]; then echo "No USB serial port found; is the clock plugged in?" >&2; exit 1; fi
echo "Using port $PORT"

case "$1" in
  upload)
    "$CLI" compile --fqbn arduino:avr:uno --upload --port "$PORT" "$HERE"
    exit $?
    ;;
  read|set) ;;
  *) sed -n '2,10p' "$0"; exit 1 ;;
esac

# Offset of local standard time from UTC, in seconds (time.timezone ignores DST).
# The RTC must hold standard time: the word clock adds the DST hour itself.
OFFSET=$(python3 -c 'import time; print(-time.timezone)')

stty -f "$PORT" 9600 raw -echo -hupcl
exec 3<>"$PORT"
# Opening the port resets the board; wait for the bootloader and setup() to finish.
perl -e 'select(undef, undef, undef, 3)'
if [ "$1" = set ]; then
  printf 'T%d\n' $(( $(date +%s) + OFFSET )) >&3
fi
perl -e '$SIG{ALRM} = sub { exit 0 }; alarm 4; $| = 1; while (defined($l = <STDIN>)) { print $l }' <&3
exec 3<&-
echo "Mac now: $(date '+%Y-%m-%dT%H:%M:%S %Z')"
