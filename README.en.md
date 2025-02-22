# RemoteControl

## Introduction

This repository is a remote desktop control software based on the RDP protocol, built upon the FreeRDP source code. It aims to provide a complete experience for domestic operating systems that are not yet fully compatible with existing software.

### Windows Installation Guide

1. xxxx
2. xxxx
3. xxxx

### Linux Installation Guide

1. xxxx

### Usage Instructions

1. Refer to the usage documentation (currently in the code improvement phase).

### Software Update Log

1. 2024.9.12: Initiated the project and drafted the project plan.
2. 2024.9.19: Completed the login interface and most of the home page (involves user connection to remote computer devices).
3. 2024.12.18: Leveraged experience from the previous Android to HarmonyOS porting project, replaced fixed-point layout with percentage-based layout, and modified the login page.
4. 2024.12.25: Updated the popup interface to a borderless design, enhancing the versatility of the popup content text.
5. 2024.12.30: Reduced the coupling of the home page interface, adopted Loader to load corresponding sub-pages, and switched most interfaces to a borderless design.
6. 2025.1.2: Added a tray icon and implemented minimize to tray and restore from tray functions (currently for Windows, further optimization needed).
7. 2025.1.12: Added a direct remote computer connection page and included a button to connect to remote computers.
8. 2025.1.25: Organized and categorized code/UI, and integrated global logger for log output.
9. 2025.2.10 Successfully connected to the database, and the login and registration function tests have been completed. On the Linux side, some connections to the FreeRDP library have been successfully established, and the initial remote connection code has been completed, pending testing. On the Windows side, a new interface for previewing locally installed applications has been added, and it is still necessary to obtain the list of local applications through the registry (including application icons, application exe file paths, and paths for uninstallation).
10. 2025.2.22 Both the Windows and Linux ends have linked the czmq library in CMake, which will be used for TCP large data transmission. On the Linux end, the relevant library functions of FreeRDP have successfully initialized the FreeRDP data. However, when using the freerdp_connect function, the program crashes for no apparent reason (a separate thread has been opened). The Windows device being connected (using my Surface Go 1st generation) has already displayed that it is in a remote connection session. The issue is still under further investigation.