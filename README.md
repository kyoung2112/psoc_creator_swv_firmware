## SWV Printf Firmware Example ##

This is an example PSoC Creator SWV Printf firmware example for PSoC3 and PSoC5LP. Target to CY8C3866AXI-040 for the PSoCe (CY8CKIT-001 or CY8CKIT-030) and CY8C5858AXI-LP035 for the PSoC5LP (CY8CKIT-001 or CY8CKIT-050). 

The current functionality redirects the printf() function for PSoC3 (Keil 8051 compiler) and PSoC5LP (gcc) to transmit bytes to the SWV stimulus port 0. Defines are included to change between manchester and UART. The output frequency for the current version is fixed to bus clk / 2 (serial output = 6 MHz at bus clk 12 MHz). Future versions will need to make this more flexible using the output dividers.

The companion C# application is at https://github.com/kyoung2112/csharp-miniprog3-swv
