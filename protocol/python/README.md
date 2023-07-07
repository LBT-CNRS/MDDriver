# MDDriver python lib

This python lib is meant to uickly interact with the mdddriver library.
It exports the standards function of the library to Python.

It works only with Python 2.X


# Installation

  - Make sure you have the environnement variables `MDDRIVER_INCLUDE_DIR` and `MDDRIVER_LIBRARY_DIR` set
  - Create a conda env with python 2.7: `conda create -n "test_mddriver" python=2.7`
  - Activate it: conda activate test_mddriver
  - Move to the `mddriver0.1/mddriver` folder where you can find the file `setup.py`
  - Install the python libraries with: `pip install -e .`

# Test

Now, you can see if the python library is working by execute 2 scripts in 2 terminals:

  - `pyton mddriverserver.py` in the 1st shell
  - `pyton mddriverclient.py` in the 2nd shell

You should see the exchange of information.
