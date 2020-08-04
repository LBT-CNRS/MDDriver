from distutils.core import setup, Extension
import sys
import os




PY3K = sys.version_info[0] > 2
USERHOME = os.getenv('USERPROFILE') or os.getenv('HOME')
MDDRIVERINCLUDEDIR=os.getenv('MDDRIVER_INCLUDE_DIR')
MDDRIVERLIBRARYDIR=os.getenv('MDDRIVER_LIBRARY_DIR')

print MDDRIVERINCLUDEDIR
__version__ = ''
for line in open('mddriver/__init__.py'):
    if (line.startswith('__version__')):
        exec(line.strip())
        break
PACKAGES = ['mddriver']

EXTENSIONS= [
#Extension("mddriver.client", sources=["mddriver/client/mddriverclient.c"],  libraries=["mddriver"], library_dirs=[MDDRIVERLIBRARYDIR], include_dirs=[MDDRIVERINCLUDEDIR]),
#Extension("mddriver.server", sources=["mddriver/server/mddriverserver.c"],  libraries=["mddriver"], library_dirs=[MDDRIVERLIBRARYDIR], include_dirs=[MDDRIVERINCLUDEDIR]), 
Extension("mddriver.mddriver", sources=["mddriver/mddriver.c"],  libraries=["mddriver"], library_dirs=[MDDRIVERLIBRARYDIR], include_dirs=[MDDRIVERINCLUDEDIR])
]
			
#PACKAGES.append('mddriver.client')
#PACKAGES.append('mddriver.server') 
#PACKAGES.append('mddriver') 


setup(name="mddriver", version="0.0",
	ext_modules =EXTENSIONS, 
	packages = PACKAGES,
	classifiers=[
                 'Development Status :: 4 - Beta',
                 'Intended Audience :: Science/Research',
                 'License :: OSI Approved :: GNU General Public License (GPL)',
                 'Operating System :: MacOS',
                 'Operating System :: Microsoft :: Windows',
                 'Operating System :: POSIX',
                 'Programming Language :: Python',
                 'Programming Language :: Python :: 2',
                 'Topic :: Scientific/Engineering :: Bio-Informatics',
                 'Topic :: Scientific/Engineering :: Chemistry'
                ]
)