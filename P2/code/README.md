# c code for the EM-FI control loop

* Control of the RIGOL signal generator DG821 (may work with other models) via the VISA Interface
* Control of the Bus Pirate for Reset generation
* Test of JTAG via Segger J-LINK

Main application in "pirate_emfi.cpp"

Put your Bus Pirate COM number in "pirate_driver.cpp"
> #define COM 7
