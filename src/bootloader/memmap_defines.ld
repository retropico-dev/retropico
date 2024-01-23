# UI    FLASH:      144772 B     16376 KB      0.86%
# NES   FLASH:      256736 B     15864 KB      1.58%
# GB    FLASH:      133108 B     15352 KB      0.85%
# SMS   FLASH:        253 KB   15720192 B      1.65%

# ~1544k used of 5Mb total reserved (see libpico2d "FLASH_TARGET_OFFSET_CACHE")

__FLASHLOADER_START = 0;
__FLASHLOADER_LENGTH = 1 * 8k;                              # 8k for bootloader
__UI_START = __FLASHLOADER_START + __FLASHLOADER_LENGTH;

# release mode
#__NES_START = __UI_START + 512k;                            # 512k for ui
#__GB_START = __NES_START + 384k;                            # 384k for nes
#__SMS_START = __GB_START + 256k;                            # 256k for gb
#__FLASHLOADER_END = __SMS_START + 1024k;                    # 384k fos sms

# debug mode
__NES_START = __UI_START + 512k;                            # 512k for ui
__GB_START = __NES_START + 1024k;                           # 1024k for nes
__SMS_START = __GB_START + 1024k;                           # 1024k for gb
__FLASHLOADER_END = __SMS_START + 1024k;                    # 1024k fos sms
