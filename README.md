# DeveloperPowerPack
Engine 2D/3D for creating games and applications that can compiled for Windows / Linux / Android and use one code base.

Library and examples at current state are designed for advanced developers who manage to build them.

You can download sources and use it as you wish for private, public and commercial purposes without any fee to me provided that:
1. you place my name and homepage:
   Marcin Załęczny (https://progmar.net.pl)
   in credits of your game or application.
3. you will not take any hostile actions goaling for forbidding me further developing the projects and use them for my own.

If you open Tilc library project (DeveloperPowerPack/libraries/CMakeLists.txt) in Visual Studio and the project you placed
on VFAT partition, then before you open it in VS you should:
1. Open the x64 Native Tools Command Prompt available in VS.
2. Navigate to directory DeveloperPowerPack/libraries/
3. Run script RequiredConfigForProjectOnVFatPartition.bat - this automatically adds required directories to git safe.directory configuration.
   This is required because VFAT does not handle users permissions opposite to NTFS and Linux filesystems. And git forces this requirement on
   when the directory is not added to safe.directory. Without this step Tilc library does not fetch dependency libraries and not compile on vfat.

If you wish to have a significant way to direct developing the library in specified direction then feel free to contact me at:
mzaleczny@gmail.com
