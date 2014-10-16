## SWV Printf Firmware Example ##

This is an example PSoC Creator SWV Printf firmware example for PSoC3 and PSoC5LP. The PSoC3 example targets the CY8C3866AXI-040 (CY8CKIT-001 or CY8CKIT-030) and the PSoC5LP example targets the CY8C5858AXI-LP035(CY8CKIT-001 or CY8CKIT-050). 

SWV is now implemented as a component. The component is entirely contained within the xxx project subdirectory. To add this component to your project, copy the directory to your project and then add the component project as a user dependency to your project (Project->Dependencies). It will show up in the Cypress Component Catalog under the "System" group. 

The only API is SWV_Start(), which needs to be called before using. The current functionality redirects the printf() function for PSoC3 (Keil 8051 compiler) and PSoC5LP (gcc) to transmit bytes to the SWV stimulus port 0. Defines are included to change between manchester and UART. The output frequency is determined by a #define in the component code.

The companion C# application is at https://github.com/kyoung2112/csharp-miniprog3-swv, and an installer of which can be downloaded at https://www.dropbox.com/s/iypxk5npp6n98mf/SWV%20Setup.exe?dl=0

TODO - Include clock speed as a component parameter.
