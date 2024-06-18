**Dev**

**1.1**

   - Declare and add `MDDriver_sendCustomInt/Float` in UnityMol-MDDriver API 
   - Add new `IIMD_get_nb_custom_int/float` dedicated function for `MDDriver_getNbCustomInt/Float`
   - Modify `MDDriver_getCustomInt/Float` to return actual size of recieved data
   - Add `IMDCustomData` structure for custom data 
     - **TO DO: Use this structure as an argument in associated custom data functions**
   - Show 'no force' warning only for `IMDmsg` log level > 0
   - In `IIMD_treatprotocol`, activate `vmd_new_custom_float` flag when recieve custom data in server side & fix `vmd_custom_int` allocation
   - Adapt `IIMD_send_custom_int/float` to send custom data from client side to server
   - Update `clienttest.c` & `servertest.c` for custom data

**1.0**

   - Create README for python library
   - Add Github Actions for building the library with the 3 OS (Linux, Mac, Windows)
   - Use Modern CMake to handle compilation & installation
   - Modify various files to fit the modern cmake usage
   - Move adapters/interaction/tools into example_tool/
   - Move adapters/UnityMol into plugins/UnityMol
   - Build by default the UnityMol plugin
   - Do not install by default the example tool
   - Fix compilations errors in the sources files of example_tool
   - Move test executable into a specific folder
   - Remove unused data files and GROMACS patches

**0.9**

    - Status of this repo before massive re-organization
