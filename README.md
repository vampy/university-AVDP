# university-PDAV
[![Build Status](https://travis-ci.org/leyyin/university-PDAV.svg?branch=master)](https://travis-ci.org/leyyin/university-PDAV)

Project for processing audio-video data http://www.cs.ubbcluj.ro/~forest/pdav/

# Build
You need to have at least `Qt 5.4` installed and configured.

## Qt Creator
1. Open the [pdav.pro](pdav.pro) file with Qt Creator.
2. You can specify the type of build targets if you click on `Details`.
3. Create a new `build` directory.
4. For `Debug` and `Release` build use the new created `build` directory.
5. Click on `Configure Project`.
6. Start hacking.
7. [Optional] If you want to use multiple cores/threads when building your project you can do this by going to the left tab called `Projects`, and under `Build Steps` on the line that says `Make: make in....` click on `Details` and in the field `Make arguments:` put `-j <number_of_threads>` (an example with 4 threads `-j 4`).

## Terminal with qmake
1. Create a build directory `mkdir build` and `cd build`.
2. Run qmake `qmake -Wall ../pdav.pro CONFIG+=debug` for debug builds or `qmake -Wall ../pdav.pro CONFIG+=release` for release builds.
3. Compile with `make -j $(nproc)` (bash shell) or `make -j (nproc)` (fish shell).
4. The executable will be in `./src/pdav`.
