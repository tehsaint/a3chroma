# DISCLAIMER

THIS IS THE BETA OF THIS MOD, AND YOU SHOULD NOT COMPILE THIS. THIS **WILL** HAVE INCONPLETE CODE.
BUILD THE .../@a3chroma INSTEAD

_________


# Chroma bridge notes

This folder contains a Windows-native bridge concept for Arma 3.

## How it works

Arma 3 calls the extension from SQF. The extension first initializes the Chroma SDK via the documented REST endpoint at http://localhost:54235/razer/chromasdk, then uses the returned instance URI to send a keyboard effect request to the local Chroma service.

## Files
- magazineempty.sqf: the SQF event hook that triggers the extension
- chroma_rest_bridge.cpp: a native Windows bridge that talks to the local Chroma REST API

## Build notes

This source is intended for a Windows build using the WinHTTP API. Compile it as a DLL and place it where Arma 3 can load it as an extension.

Usually : ..\arma3\@ModXYZ\yourextension.dll or ..\arma3\yourextension.dll
https://community.bistudio.com/wiki/Extensions#Preamble

## Expected behavior

- init -> posts to /razer/chromasdk and stores the returned instance URI
- flash_red_burst -> posts a static keyboard effect to the instance URI so the local Chroma service can display a red burst-style effect
