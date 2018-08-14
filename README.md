This silent command generator enables you to create a proxy executable that would call out 
to a Windows Console Subsystem process _without_ Windows spawning a console window in the
process, hence the "silent" command.

This is useful for calling executables with macro keys.

There is an inherent danger in this, and must be utilized wisely.

The program requires the MSVC toolchain to be installed as a dependency.
I use Visual Studio Community 2017.
