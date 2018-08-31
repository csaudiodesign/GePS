# GePS

Gesture based Performance System

_Using a data glove as Musical Instrument_

For more information: <http://geps.synack.ch>

## Dependencies

Since version 4.2, the pure data GePS application is compatible with __vanilla Pd__ (tested with Pd-0.48.1).
The application depends on libraries, abstractions and externals from 3rd party developers, which have to be installed before using this application:

- cyclone
- list-abs
- freeverb~

Since Pd-0.48 you can use the built-in deken plugin to install these dependencies (menu "Help/Find Externals...").

You don't have to add the libraries to the startup arguments of puredata, as long as they are installed in the pure data externals path. The `settings`-subpatcher contains a declaration to load the cyclone library (`[declare -lib cyclone]`), and the abstractions are referenced with the corresponding path (`[list-abs/list-tabdump xxx]`).
