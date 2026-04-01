### Prerequisites
Run:
```
sudo apt install -y inotify-tools meson libglew-dev libsdl2-ttf-dev
```

### Build
Run:
```
meson setup build
cd build
meson compile
```

Put lotos-screensaver binary, start-idle.sh and start-watch.sh into /usr/local/bin/ directory.
Create user service directory. Put lotos-idle.service, lotos-watch.service and lotos-screensaver.service there and run it.
```
mkdir -p ~/.config/systemd/user
cp config/systemd/lotos-idle.service ~/.config/systemd/user/
cp config/systemd/lotos-watch.service ~/.config/systemd/user/
cp config/systemd/lotos-screenasaver.service ~/.config/systemd/user/
systemctl --user daemon-reload
systemctl --user enable lotos-idle.service
systemctl --user enable lotos-watch.service
systemctl --user enable lotos-screensaver.service
systemctl --user start lotos-idle.service
systemctl --user start lotos-watch.service
```

Configuration file `~/Documents/config/config.json`.
Media files `~/.config/lotos-screensaver/media`.
