# Edge RTSP Object Detection (CVITEK)

Real-time RTSP object detection on CVITEK SoCs using hardware decode/encode and YOLOv8 (CVI_TDL). The app ingests an RTSP stream, runs detection and tracking, draws boxes, then republishes the annotated video as an RTSP stream.

## What It Does
- Pulls an RTSP stream and decodes it with the CVITEK hardware decoder.
- Runs YOLOv8 detection via CVI_TDL and tracks objects with ByteTrack.
- Draws bounding boxes on the frame.
- Re-encodes with the CVITEK hardware encoder and serves an RTSP output stream.

## Pipeline
RTSP in -> HW decode -> YOLOv8 (CVI_TDL) -> ByteTrack -> draw -> HW encode -> RTSP out

## Requirements
- CVITEK SDK components: CVI_TDL, CVI_VDEC, CVI_VENC, CVI_SYS, CVI_VB
- C++17 toolchain and CMake
- OpenCV
- FFmpeg headers/libs (libavformat, libavutil)
- CVITEK SoC (for example CV180x or CV182x)

## Build
```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

## Run
```bash
./app_AI_decode <model_path> [rtsp_url]
```

If `rtsp_url` is omitted, the default value in `app/stream.h` is used.

## Output Stream
The application starts a local RTSP server and publishes a stream named `cam1` on port `8854`.

## Project Layout
- `app/`: entry points and example apps
- `gvision/`: RTSP reader, hardware decoder/encoder, AI detector, system init
- `cmake/`: CMake modules for CVITEK, OpenCV, FFmpeg, ByteTrack

## Notes
- This project targets CVITEK hardware; it will not run correctly on non-CVITEK devices.
- The detection threshold defaults are set in code and can be adjusted in `app/main.cc`.
