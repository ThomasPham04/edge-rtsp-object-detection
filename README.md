# Edge RTSP Object Detection on CVITEK

Real-time RTSP object detection for CVITEK/LicheeRV Nano class boards. The app reads an RTSP stream, decodes it with the CVITEK hardware decoder, runs YOLOv8 detection through CVI_TDL, tracks detected people with ByteTrack, draws boxes, re-encodes the frame with the hardware encoder, and republishes the result as an RTSP stream.

## Pipeline

```text
RTSP input -> FFmpeg -> CVI_VDEC -> CVI_TDL YOLOv8 -> ByteTrack -> draw boxes -> CVI_VENC -> RTSP output
```

## Repository Layout

- `app/`: application entry points.
- `gvision/rtsp_reader/`: FFmpeg RTSP packet reader.
- `gvision/decoder/`: CVITEK hardware decoder wrapper.
- `gvision/encoder/`: CVITEK hardware encoder and RTSP output wrapper.
- `gvision/AI/`: CVI_TDL model loading and detection wrapper.
- `gvision/sys/`: CVI SYS/VB initialization.
- `cmake/`: dependency and toolchain CMake helpers.
- `set_env.sh`: local SDK/toolchain environment setup.

## External Dependencies

This repository does not vendor the large SDKs and binary libraries. Install or place them beside this repo, then point CMake to them with environment variables if needed.

For full install and deployment steps, see [REQUIREMENTS.md](REQUIREMENTS.md).

| Dependency | Used For | Default Path |
| --- | --- | --- |
| CVITEK middleware SDK | `CVI_SYS`, `CVI_VB`, `CVI_VDEC`, `CVI_VENC`, ISP/support libs | `../middleware/v2` |
| CVI TDL SDK | YOLOv8 inference, image processor, draw rect, RTSP helper libs | `../cvitek_tdl_sdk` |
| CVITEK TPU/IVE libs | Runtime acceleration used by CVI_TDL | under `../cvitek_tdl_sdk/sample/3rd` |
| FFmpeg | RTSP input demuxing and packets | `../ffmpeg_libs_n4.4.4.2` |
| ByteTrack-cpp | Object tracking | `../ByteTrack-cpp` |
| RISC-V musl toolchain | Cross-compilation | `../host-tools/gcc/riscv64-linux-musl-x86_64` |
| Eigen | ByteTrack headers | `/usr/include/eigen3` |

The expected local layout is:

```text
LicheeRV-Nano-Build/
  ByteTrack-cpp/
  cvitek_tdl_sdk/
  ffmpeg_libs_n4.4.4.2/
  host-tools/
  middleware/
  libav-decoder/
```

If your folders are somewhere else, override them:

```bash
export TOOLCHAIN_DIR=/path/to/riscv64-linux-musl-toolchain
export MW_PATH=/path/to/middleware/v2
export CVI_TDL_DIR=/path/to/cvitek_tdl_sdk
export FFMPEG_ROOT=/path/to/ffmpeg_libs
export BYTETRACK_PATH=/path/to/ByteTrack-cpp
```

## Build

From the repository root:

```bash
source ./set_env.sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/riscv-toolchain.cmake
cmake --build build -j
```

The executable is produced at:

```text
build/app/app_AI_decode
```

## Run

Pass the model path and RTSP URL:

```bash
./build/app/app_AI_decode /path/to/yolov8.cvimodel 'rtsp://user:pass@host/Streaming/channels/101'
```

Or set the RTSP URL through the environment:

```bash
export RTSP_URL='rtsp://user:pass@host/Streaming/channels/101'
./build/app/app_AI_decode /path/to/yolov8.cvimodel
```

The app starts an RTSP output server on port `8854` and publishes the annotated stream at:

```text
rtsp://<board-ip>:8854/cam1
```

## Runtime Notes

- The input stream must be H.264 or H.265.
- The output stream is encoded as H.264.
- Current tracking code filters class `0`, which is the person class for COCO-style YOLO models.
- Detection thresholds are set in `app/main.cc`.
- This project requires CVITEK hardware; it is not expected to run correctly on a normal PC.

## What Not To Commit

Do not commit:

- RTSP URLs with usernames/passwords.
- `.env` files.
- SDK folders, toolchains, generated build directories, or board-specific binary drops.

Keep SDKs and binary dependencies outside the repo and document their versions. ByteTrack-cpp may be added as a submodule if your license and project policy allow it.

## Troubleshooting

- If CMake cannot find `cvi_sys.h` or `linux/cvi_comm_venc.h`, check `MW_PATH`.
- If CMake cannot find FFmpeg headers or libs, check `FFMPEG_ROOT`.
- If `BYTETracker.h` is missing, check `BYTETRACK_PATH` and ensure ByteTrack-cpp was built for RISC-V.
- If the app prints repeated decoder frame collection errors, verify the RTSP stream is stable and that the input codec is H.264/H.265.
- If the output stream opens but shows no video, confirm the board has enough VB memory and that `CVI_VENC` starts successfully.
