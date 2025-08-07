#pragma once
#include <cvi_sys.h>
#include <cvi_vb.h>
#include <cvi_comm_video.h>
#include <cstring>
#include <iostream>

class SystemInit{
    public:
    static bool init(int srcWidth, int srcHeight);
};