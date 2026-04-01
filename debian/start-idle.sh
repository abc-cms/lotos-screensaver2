#!/bin/bash

CONFIG="$HOME/Documents/config/config.json"

TIMEOUT=60

if [ -f "$CONFIG" ]; then
    TIMEOUT=$(jq -r '.screensaver_settings.inactivity_timeout // 60' "$CONFIG")
fi

if ! [[ "$TIMEOUT" =~ ^[0-9]+$ ]]; then
    TIMEOUT=60
fi

exec swayidle -w \
  timeout "$TIMEOUT" '/usr/local/bin/lotos-screensaver' \
  resume 'pkill -f /usr/local/bin/lotos-screensaver'
  