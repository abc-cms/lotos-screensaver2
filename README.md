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

Configuration file `~/.config/lotos-screensaver/config/config.yaml`.
Media files `~/.config/lotos-screensaver/media`.
