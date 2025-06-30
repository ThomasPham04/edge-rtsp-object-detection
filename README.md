### Install dependencies
#### install OpenCV
```shell
sudo apt update
sudo apt install libopencv-dev
```
### Install ffmpeg
```shell
sudo apt update
sudo apt install libavcodec-dev libavformat-dev libswscale-dev libavutil-dev libavdevice-dev libavfilter-dev

```
### Build
```shell
mkidr build && cd build && cmake .. && make -j4
```
