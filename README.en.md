# RemoteControl

#### Introduction

This repository is a remote desktop control software based on the RDP protocol, derived from the FreeRDP source code, aimed at providing a complete experience for software not originally adapted for domestic operating systems on domestic operating systems.

#### Installation Guide

1. xxxx
2. xxxx
3. xxxx

#### Usage Instructions

1. CMake version >= 3.20.0
2. Qt version >= 6.5.3

#### Software Update Log

1. **2024.9.12** Launched the project and drafted the project plan.
2. **2024.9.19** Completed the login interface and most of the main page (involving user connections to remote computer devices).
3. **2024.12.18** Leveraged experience from the previous Android porting to HarmonyOS project, replaced the original fixed-point layout with a percentage-based layout, and modified the login page.
4. **2024.12.25** Modified the pop-up interface, adopting a borderless design, enhancing the versatility of the text content in the pop-up.
5. **2024.12.30** Reduced the coupling of the main page interface, using Loader to load corresponding sub-pages. Most interfaces have switched to a borderless design.
6. **2025.1.2** Added a tray icon, and added the functionality of minimizing to the tray and restoring from the tray (Windows-specific functionality, needs further optimization).
7. **2025.1.12** Added a page for directly connecting to a remote computer, and added a button to connect to a remote computer.
8. **2025.1.25** Organize and categorize code/UI, and integrate global logger for log output.
9. **2025.2.10** Successfully connected to the database, and the login and registration function tests have been completed. On the Linux side, some connections to the FreeRDP library have been successfully established, and the initial remote connection code has been completed, pending testing. On the Windows side, a new interface for previewing locally installed applications has been added, and it is still necessary to obtain the list of local applications through the registry (including application icons, application exe file paths, and paths for uninstallation).
10. **2025.2.22** Both the Windows and Linux ends have linked the czmq library in CMake, which will be used for TCP large data transmission. On the Linux end, the relevant library functions of FreeRDP have successfully initialized the FreeRDP data. However, when using the freerdp_connect function, the program crashes for no apparent reason (a separate thread has been opened). The Windows device being connected (using my Surface Go 1st generation) has already displayed that it is in a remote connection session. The issue is still under further investigation.

