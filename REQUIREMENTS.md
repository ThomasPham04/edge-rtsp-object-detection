# Requirements and Installation

This project is a cross-compiled CVITEK hardware application. A clone of this repo is not enough by itself; the host machine must have the CVITEK SDK/toolchain and the target board must have the matching runtime libraries.

## 1. Host Machine

Use an Ubuntu/Linux host for building. Install the common build tools:

```bash
sudo apt update
sudo apt install -y \
  build-essential \
  cmake \
  git \
  make \
  ninja-build \
  pkg-config \
  libeigen3-dev
```

CMake 3.20 or newer is recommended because the top-level project requires it.

## 2. Required SDKs and Libraries

Obtain these packages from your board/vendor SDK release or internal build artifacts:

- RISC-V musl toolchain for the CVITEK target.
- CVITEK middleware SDK containing headers/libs for `CVI_SYS`, `CVI_VB`, `CVI_VDEC`, `CVI_VENC`, ISP, and support libraries.
- CVI TDL SDK containing `cvi_tdl`, `cvi_tdl_app`, preprocessing, draw-rect, TPU/IVE, OpenCV, and RTSP helper libraries.
- FFmpeg headers/libs built for the same RISC-V musl target.
- ByteTrack-cpp source, built as a RISC-V shared library.
- A YOLOv8 `.cvimodel` compatible with CVI_TDL.

The default expected folder layout is:

```text
LicheeRV-Nano-Build/
  ByteTrack-cpp/
  cvitek_tdl_sdk/
  ffmpeg_libs_n4.4.4.2/
  host-tools/
  middleware/
  libav-decoder/
```

The project derives default paths from that layout. If your folders are elsewhere, export these variables before configuring:

```bash
export TOOLCHAIN_DIR=/path/to/riscv64-linux-musl-toolchain
export MW_PATH=/path/to/middleware/v2
export CVI_TDL_DIR=/path/to/cvitek_tdl_sdk
export FFMPEG_ROOT=/path/to/ffmpeg_libs
export BYTETRACK_PATH=/path/to/ByteTrack-cpp
```

## 3. Verify SDK Paths

After placing/extracting the SDKs, these checks should pass:

```bash
cd /path/to/LicheeRV-Nano-Build/libav-decoder
source ./set_env.sh

test -x "${TOOLCHAIN_DIR:-../host-tools/gcc/riscv64-linux-musl-x86_64}/bin/riscv64-unknown-linux-musl-g++"
test -f "${MW_PATH}/include/cvi_sys.h"
test -f "${MW_PATH}/include/linux/cvi_comm_venc.h"
test -f "${CVI_TDL_DIR}/include/cvi_tdl/cvi_tdl.h"
test -f "${FFMPEG_ROOT}/include/libavformat/avformat.h"
```

If one of these fails, fix the corresponding SDK path before building.

## 4. Build ByteTrack-cpp for RISC-V

The app links against `libbytetrack.so`, so ByteTrack must be built for the board architecture, not for the host PC.

From `LicheeRV-Nano-Build/`:

```bash
cmake -S ByteTrack-cpp -B ByteTrack-cpp/build_riscv \
  -DCMAKE_TOOLCHAIN_FILE=libav-decoder/cmake/riscv-toolchain.cmake \
  -DBUILD_BYTETRACK_TEST=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DEigen3_DIR=/usr/share/eigen3/cmake

cmake --build ByteTrack-cpp/build_riscv -j
```

Verify the output architecture:

```bash
file ByteTrack-cpp/build_riscv/libbytetrack.so
```

Expected output should mention `UCB RISC-V`. If it says `x86-64`, delete the build directory and configure again with the toolchain file.

## 5. Build This Project

From `libav-decoder/`:

```bash
source ./set_env.sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/riscv-toolchain.cmake
cmake --build build -j
```

Expected output:

```text
build/app/app_AI_decode
build/gvision/libgvision.so
```

Verify the executable architecture:

```bash
file build/app/app_AI_decode
```

Expected output should mention `UCB RISC-V`.

## 6. Install/Deploy to the Board

Copy at least these files to the CVITEK board:

```text
build/app/app_AI_decode
build/gvision/libgvision.so
../ByteTrack-cpp/build_riscv/libbytetrack.so
/path/to/your/yolov8.cvimodel
```

The board root filesystem must also provide the CVITEK middleware, CVI_TDL, TPU/IVE, RTSP, OpenCV, and FFmpeg runtime `.so` libraries. If they are not already in the board image, copy the required `.so` files from the SDK library directories and set `LD_LIBRARY_PATH`.

Example on the board:

```bash
export LD_LIBRARY_PATH=/path/to/libs:$LD_LIBRARY_PATH
export RTSP_URL='rtsp://user:pass@camera-host/Streaming/channels/101'
./app_AI_decode /path/to/yolov8.cvimodel
```

The annotated stream is published at:

```text
rtsp://<board-ip>:8854/cam1
```

## 7. Common Build Failures

- `cvi_sys.h: No such file or directory`: `MW_PATH` is wrong or middleware SDK is missing.
- `cvi_tdl.h: No such file or directory`: `CVI_TDL_DIR` is wrong or CVI TDL SDK is missing.
- `libavformat/avformat.h: No such file or directory`: `FFMPEG_ROOT` is wrong or FFmpeg headers are missing.
- `BYTETracker.h: No such file or directory`: `BYTETRACK_PATH` is wrong.
- Linker reports incompatible `libbytetrack.so`: ByteTrack was built for the host architecture; rebuild it with the RISC-V toolchain.
- App starts but no output stream: check board runtime `.so` paths, VB memory, RTSP input stability, and whether `CVI_VENC` started successfully.

## 8. Notes for Maintainers

Do not commit SDK folders, toolchains, generated `build/` directories, `.env` files, or RTSP URLs with credentials. Keep large binary dependencies external and document their versions alongside a release.
