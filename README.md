# university-PDAV
Project for processing audio-video data http://www.cs.ubbcluj.ro/~forest/pdav/

# Install
You need to have at least `Qt 5` installed and configured.

## Qt Creator
1. Open the [pdav.pro](pdav.pro) file with Qt Creator.
2. You can specify the type of build targets if you click on `Details`.
3. Create a new `build` directory.
4. For `Debug` and `Release` build use the new created `build` directory.
5. Click on `Configure Project`.
6. Start hacking.
7. [Optional] If you want to use multiple cores/threads when building your project you can do this by going to the left tab called `Projects`, and under `Build Steps` on the line that says `Make: make in....` click on `Details` and in the field `Make arguments:` put `-j <number_of_threads>` (an example with 4 threads `-j 4`).
