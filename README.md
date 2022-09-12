## Installation

Install [libcamera](https://github.com/kbingham/libcamera)

Clone Projekt

Run createLinks.sh

Create Build Folder
```sh
mkdir build
```

Build on Linux
```sh
cd build
cmake ..
cmake --build
```

Build on Raspberry Pi
```sh
cd build
cmake ..
cmake --build -DCOMPILE_FOR_RASPBERRY=1
```

Run on Linux
```sh
./cam_code
```

Run on Raspberry Pi
(GPIO needs root)
```sh
sudo ./cam_code
```