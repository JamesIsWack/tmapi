The StringDatabase example includes a win32 server application (StringServer),
some PPU example code (TargetExample) and an autoexec.eic startup script.

To get this sample to run you must:

1) Run 'Server.exe' by opening up 'StringServer\Server.sln' and hitting F5
2) Move the autoexec.eic script into the directory the debugger is run from.
3) Open the 'TargetExample\Target_StringID.sln' and hit F5.
4) Stop the execution in ProDG at any point and hover over the myStringID variable. A request is made
to the server for a string which pairs with the current value of the variable. In a watch, locals
or autos view you can use the Database context menu for your myStringID variable to view this
same string.

Both the command view and server.exe window should display helpful debug messages.