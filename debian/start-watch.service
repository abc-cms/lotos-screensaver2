#!/bin/bash

CONFIG_DIR="$HOME/Documents/config"
CONFIG_FILE="config.json"
SERVICE="lotos-idle.service"

echo "Watching $CONFIG_DIR/$CONFIG_FILE ..."

inotifywait -m -e close_write -e moved_to -e create "$CONFIG_DIR" |
while read path action file; do
    if [[ "$file" == "$CONFIG_FILE" ]]; then
        echo "$(date '+%F %T') - Detected change in $file ($action), restarting $SERVICE"
        systemctl --user restart "$SERVICE"
    fi
done
