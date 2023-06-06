//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_INPUT_LINUX_H
#define MICROBOY_INPUT_LINUX_H

namespace mb {
    class LinuxInput : public Input {
    public:
        uint16_t getButtons() override;
    };
}

#endif //MICROBOY_INPUT_LINUX_H
