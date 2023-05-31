//
// Created by cpasjuste on 30/05/23.
//

#include "platform_linux.h"
#include "display_linux.h"

using namespace mb;

LinuxPlatform::LinuxPlatform() : Platform() {
    p_display = new LinuxDisplay();
}
