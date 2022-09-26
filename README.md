## Installation

For Raspberry:
Install [libcamera](https://github.com/kbingham/libcamera) & X11 headers

For Linux:
Install X11 header

Clone Projekt

Run createLinks.sh on Raspberry

Create Build Folder
```sh
mkdir build
```

Build on Linux/Windows
```sh
cd build
cmake ..
cd ..
cmake --build ./build
```

Build on Raspberry Pi
```sh
cd build
cmake ..
cd ..
cmake --build ./build -DCOMPILE_FOR_RASPBERRY=1
```

Run on Linux
```sh
./<Path to programm>/cam_code -preview -lines
```

Run on Windows
```sh
./<Path to programm>/cam_code.exe -preview -lines
```

Run on Raspberry Pi
(GPIO needs root)
```sh
sudo ./<Path to programm>/cam_code
```

# Info
Args:
- "-preview"
- "-lines"
- "-record"
