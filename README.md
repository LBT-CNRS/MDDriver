# MDDriver

MDDriver is a library allowing a user to easily couple molecular visualisation tools with simulation code through a network. It allows to carry out interactive molecular dynamics, visualize simulation results and drive a simulation in interactive time.

**The previous [sourceforge repository](https://sourceforge.net/projects/mddriver/) is now deprecated and the latest version is on github.**

[![](https://img.youtube.com/vi/tHGXYcgRqc8/0.jpg)](https://www.youtube-nocookie.com/embed/tHGXYcgRqc8)
![](https://a.fsdn.com/con/app/proj/mddriver/screenshots/page16-1008-full.jpg/245/183/1.5)
![](https://a.fsdn.com/con/app/proj/mddriver/screenshots/page16-1000-full.jpg/245/183/1.5)
![](https://a.fsdn.com/con/app/proj/mddriver/screenshots/page16-1004-full.jpg/245/183/1.5)



This library is needed for:
   - Interactive Simulation with [Unitymol](https://github.com/LBT-CNRS/UnityMol-Releases) as a visualisation tool.
   - Interactive simulation with the [Biospring tool](https://sourceforge.net/projects/biospring/) as a simulation engine.


IMD (Interactive Molecular Dynamics) is supported natively in GROMACS and VMD. MDDriver is no longer needed for this kind of simulation.
**MDDriver is compatible with this IMD protocol used by VMD and GROMACS.**

These 3 components (UnityMol, MDDriver and Biospring) have been used together recently to position rapidely and interactively proteins within membranes [1].

## Compilation & Installation

### Requirements

- CMake [ http://www.cmake.org]( http://www.cmake.org)

### Compilation

```
git clone https://github.com/LBT-CNRS/MDDriver
cd MDDriver/

# On Windows with Microsoft Visual Studio 16 for example
cmake -S . -B build -DCMAKE_INSTALL_PREFIX:PATH=/path/to/mddriver/installation -G "Visual Studio 16 2019"

# On Mac/Linux
cmake -S . -B build -DCMAKE_INSTALL_PREFIX:PATH=/path/to/mddriver/installation
cmake --build build
cmake --install
```

By default, the UnityMol plugin is compiled. This can be deactivated with `-DBUILD_PLUGINS=OFF`.

By default, the example tool in C++ and the python library is not installed. This can be activated with `-DINSTALL_EXAMPLE_TOOL=ON` and `-DINSTALL_PYTHON_LIB=ON`.

### Test

You can test the protocol by lauching 2 executables, one in each console:

First console:

    /path/to/mddriver/installation/bin/servertest

Second console:

    /path/to/mddriver/installation/bin/clienttest

You should see energy and coords exchanging by the server and the client.

### Usage

Usually, MDDriver is used as a library for another code.

**For Biospring:** give the path to the MDDriver installation when compiling it

**For UnityMol:** Copy/paste the library file into the `Plugins` folder of the UnityMol build.


## References

[1]: Lanrezac, A.; Laurent, B.; Santuz, H.; Férey, N.; Baaden, M. Fast and Interactive Positioning of Proteins within Membranes. Algorithms 2022, 15, 415. https://doi.org/10.3390/a15110415

## License

MDDriver is licenced under the [CeCILL-C license](LICENSE.txt).

## Contributors :

- Marc Baaden
- Xavier Martinez
- Hubert Santuz
- André Lanrezac
- Nicolas Ferey
- Olivier Delalande
- Gilles Grasseau
