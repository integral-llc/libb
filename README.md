# libb
An utility to properly create a bundle for Cocoa applications. 

It uses `otool` application to analyze needed dependencies 
from the application itself and its dependencies recursively. 

It will copy all needed files to `bundle/Frameworks` folder
and change needed links using `install_name_tool -change` command.


