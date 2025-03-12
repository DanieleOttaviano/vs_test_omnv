# Steps to repeat the experiments on the arm board (tested on Kria KV260)

Clone the MARTe2 and MARTe2-components togheter with the current repo:
```sh
git clone --recurse-submodules https://github.com/DanieleOttaviano/vs_test_omnv.git
```
Or if you’ve already cloned the repo:
```sh
git submodule update --init --recursive
```

Initialize the bash variables using the setup_marte.sh script:
WARNING: Check the script and set the target for rsync.
```sh
source setup_marte.sh
```

Patch the MARTe2 directory to avoid compiling errors,
copy the custom GAMs and compile the MARTe2 Core and examples:
```sh
cd ${MARTe2_DIR}
git apply ../patches/marte2_omnv_exp.patch
cp ../custom_GAMs/MARTe2_Docs_User_source__static_examples_Core/* Docs/User/source/_static/examples/Core/
make -f Makefile.gcc core
cd ${MARTe2_DIR}/Docs/User/source/_static/examples/Core
make -f Makefile.gcc
``

Compile the MARTe2-components Core:
```sh
cd ${MARTe2_Components_DIR}
git apply ../patches/marte2_components_omnv_exp.patch
make -f Makefile.gcc core
```

Be sure that the IP and ports are correct in the Configurations files:
```sh
cat target/Configuration/VS-Control.cfg
```
or
```sh
cat target/Configuration/VSES-Control.cfg
```

Copy the Builds directories, the scripts and the configuration on the target board:
```sh
./load_target.sh
```

Access the target board and use the setup_platform.sh script to 
1) remove the scheduler protection from throttling
2) remove all the irq to the core running the real-time thread
3) Assign only the eth interrupt to the core running the real-time thread
WARNING: Check the interrupt numbers on the target with "cat /proc/interrupts"
         Make sure the irq related to the network is assigned to the core.
```sh
./setup_board.sh
```

On the target start the MARTe2 VS application:
```sh
./MARTeApp.sh -l RealTimeLoader -s State1 -f ./Configuration/VS-Control.cfg
```
or (if extremum seeking)
```sh
./MARTeApp.sh -l RealTimeLoader -s State1 -f ./Configuration/VSES-Control.cfg
```

On the server you can compile compile the application that sends the data to
the target using udp (there must be a la cable that connects server and target) 
```sh
cd server_udp
gcc vs_udp.c -o vs_udp
```
or
```sh
cd server_udp
gcc vses_udp.c -o vses_udp
```

Send the data to the target using the following application:
```sh
sudo chrt -f 99 ./vs_udp
```
or (if extremum seeking)
```sh
sudo chrt -f 99 ./vses_udp
```
