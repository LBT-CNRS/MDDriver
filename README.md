# MDDriver

MDDriver is a library allowing a user to easily couple molecular visualisation tools with simulation code through a network. It allows to carry out interactive molecular dynamics, visualize simulation results and drive a simulation in interactive time.

The previous [sourceforge repository](https://sourceforge.net/projects/mddriver/) is now deprecated and the latest version is on github.

[![](https://img.youtube.com/vi/tHGXYcgRqc8/0.jpg)](https://www.youtube-nocookie.com/embed/tHGXYcgRqc8)
![](https://a.fsdn.com/con/app/proj/mddriver/screenshots/page16-1008-full.jpg/245/183/1.5)
![](https://a.fsdn.com/con/app/proj/mddriver/screenshots/page16-1000-full.jpg/245/183/1.5)
![](https://a.fsdn.com/con/app/proj/mddriver/screenshots/page16-1004-full.jpg/245/183/1.5)

## Compilation

```
git clone https://github.com/LBT-CNRS/MDDriver
cd MDDriver/
# On Windows (Visual Studio 2019)
#cmake -S . -B build -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE="Release" -A x64
# On Mac/Linux
cmake -S . -B build -DCMAKE_BUILD_TYPE="Release" -A x64

cmake --build build --config Release
```

## License

CeCILL-C license compatible with the LGPL.

