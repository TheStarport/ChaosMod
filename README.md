# bmod-hooks

A series of plugins for Freelancer. These should be written into the `[Libraries]` section of dacom.ini and dacomsrv.ini as appropriate.

## BmodOffsets

This plugin does the following:
- Patches Freelancer.exe to regenerate restart.fl on every load of the game.
- Sets a number of offsets from the [Limit Breaking 101 page](https://wiki.the-starport.net/FL%20Binaries/limit-breaking/).

The offsets defined in this plugin are set to values specific to the Better Modernised Combat Mod, but can be adjusted, repurposed and added very easily by simply calling the `PatchM` and `PatchV` functions as appropriate.
