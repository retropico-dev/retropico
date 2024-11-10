<h1 align="center">RetroPico</h1>

<p align=center>
  <img src="https://github.com/retropico-dev/retropico/raw/dev/docs/pics/retropico-pic-01.jpg" width="50%" height="50%" alt="RetroPico !">
</p>

RertroPico is "yet another" rp2040 handeld deivce.

The whole project was done by myself, and I'm very proud of it. This includes familiarizing with microcontrollers, writing a cross platform framework for linux and the chosen one (rp2040), porting some emulators, designing a pcb and finally buying, designing and printing a 3d case for it.

The main goal was to make a cheap handheld capable of running a few 8 bit consoles on a homemade tiny device, which can be "easily" (with a few soldering skills) done at a reasonable price. Finally, it would be totally open source, of course. The cost of the device is currently ~30€ for the components (and shipping). Building instructions and documentation is in the work.

The "retropico" os currently support four 8 bits consoles inducing the nes, gameboy, master system and game gear. While a lot of work has been put into porting the emulated systems (including 60fps, sram support, ...), the emulation is far from perfect. A few games won't run, more will have graphical glitches, but the fun is here.

### PCB

TOP | BOT
:--:|:--:
![](docs/pics/PCB_RetroPico-top-v1.2_2024-11-10.svg) | ![](docs/pics/PCB_RetroPico-bottom-v1.2_2024-11-10.svg)

I use [JetBrains CLion](https://www.jetbrains.com/clion/) IDE for all my open sources projects, thanks for the support !
