# How to compile DSDL for DroneCAN
1. Clone the [dronecan_dsdlc repository](https://github.com/aralce/dronecan_dsdlc)
2. Install empy python module with: "pip install empy"
3. Install pexpect python module with: "pip install pexpect"
4. Clone the [DSDL repository](https://github.com/aralce/DSDL)
5. Clone the [libcanard repository](https://github.com/aralce/libcanard)
6. Check the folders are on the same directory
7. Open a terminal on the folder with the repositories.
8. Run the following command on the terminal: python3 dronecan_dsdlc/dronecan_dsdlc.py -O dsdlc_generated DSDL/ardupilot DSDL/uavcan

After these steps the folder ***dsdlc_generated*** should be created with the source files to be used with libcanard.

----------------------------------------------------------------------------------
# [Back](https://github.com/aralce/dronecan_with_libcanard/blob/master/README.md)
