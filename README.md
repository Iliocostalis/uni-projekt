## Installation

#Raspberry:
Install [libcamera](https://github.com/kbingham/libcamera) & X11 headers (libx11-dev)
Note: Building libcamera needs more ram than the Raspberry 3B+ has. To fix it increase the "Swap File" size (1024 MB is enough).
Export the path to libcamera.
Example:
```sh
export LIBCAMERA=/home/dragon/Desktop/libcamera
```

#Linux:
Install X11 header

#Windows:
None

Clone Projekt

Create Build Folder
```sh
mkdir build
```

Build on Linux/Windows
```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cd ..
cmake --build ./build
```

Build on Raspberry Pi
```sh
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCOMPILE_FOR_RASPBERRY=1
cd ..
cmake --build ./build
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
