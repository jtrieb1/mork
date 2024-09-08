# Welcome to Mork
Mork is a library for building Zork-like games in C. It is currently a work in progress.

## Building
Run `make` in the root directory and the various tools associated with Mork will
appear in the `bin` directory. The packaged library will appear in the `build`
directory in both static and dynamic formats. If you run `make install`, the Mork 
library will copy itself into your `/usr/lib` folder and the headers will be placed 
into `/usr/include/mork`. Of course, you may need to use `sudo`. Running 
`make uninstall` will reverse this.
