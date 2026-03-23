### Prerequisites
Run:
```
sudo apt install -y meson libglew-dev libsdl2-ttf-dev
```

### Build
Run:
```
meson setup build
cd build
meson compile
```

### Service
config/systemd/lotos-idle.service

Put lotos-idle and lotos-screensaver binaries into /usr/local/bin/ directory.
Create user service directory. Put lotos-idle.service there and run it.
```
mkdir -p ~/.config/systemd/user
cp config/systemd/lotos-idle.service ~/.config/systemd/user/
systemctl --user daemon-reload
systemctl --user enable lotos-idle.service
systemctl --user start lotos-idle.service
```

Configuration file `~/Documents/config/config.json`.
Media files `~/.config/lotos-screensaver/media`.
