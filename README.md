# CHIP-8 Emulator

My first take at an emulator. Using the following resources to help:

- [This dude's blog](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
- [Opcode manual](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.0)
- [Good ol' wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

## Building

`make` lol

## Running

The only caveat of running is that, for now, the program needs to be run with root permissions otherwise you cannot access the input devices and read from the keyboard... Until I find a better way to handle it. There is the potential for `udev` rules, but I don't really like that either.

## Other Resources

### Keyboard information

- [Linux input.h header](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input.h)
- [ioctl used for finding device info](http://man7.org/linux/man-pages/man2/ioctl.2.html)
- [article on discovering input](https://www.linuxjournal.com/article/6429)
- [This android example](https://android.googlesource.com/device/generic/brillo/+/d1917142dc905d808519023d80a664c066104600/examples/keyboard/keyboard_example.cpp)
