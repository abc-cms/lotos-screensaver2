#!/bin/bash

CONFIG="$HOME/Documents/config/config.json"

TIMEOUT=60

if [ -f "$CONFIG" ]; then
    TIMEOUT=$(jq -r '.screensaver_settings.inactivity_timeout // 60' "$CONFIG")
fi

if ! [[ "$TIMEOUT" =~ ^[0-9]+$ ]]; then
    TIMEOUT=60
fi

swayidle -w \
  timeout "$TIMEOUT" "systemctl --user start lotos-screensaver.service" \
  resume "systemctl --user stop lotos-screensaver.service"
