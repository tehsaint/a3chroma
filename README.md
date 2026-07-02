# a3chroma

Proof-of-concept Arma 3 integration for Razer Chroma.

## Structure

- addons/chroma_rest_bridge.cpp: C++ source for the Windows DLL bridge
- addons/magazineempty.sqf: SQF handler that calls the extension
- addons/initPlayerLocal.sqf: simple entry point for testing
- addons/config.cpp: minimal Arma addon config
- mod.cpp: basic mod metadata

## Testing notes

1. Build the DLL and name it `chroma_rest_bridge.dll`.
2. Place the DLL in the same folder as the addon files or in a location your Arma mod loads from.
3. Pack the addon folder into a PBO for Arma 3.
4. Load the mod in Arma 3 and join a mission to trigger the script.
