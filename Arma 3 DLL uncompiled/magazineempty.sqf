// Simple Chroma test: when the player fires a weapon, flash the keyboard white for 50 ms.
// Put this in your mission folder and run it from initPlayerLocal.sqf.
//
// This is meant to fo two simply things:
// 1. On startup, initialize the Chroma extension.
// 2. On a real fire event, trigger a short white flash.

private _result = "chromaexecutor" callExtension "init";

if (_result == "ok") then {
    hint "Chroma initialized successfully!";
} else {
    hint format ["Chroma init failed: %1", _result];
};

player addEventHandler ["Fired", {
    params ["_unit", "_weapon", "_muzzle", "_mode", "_ammo", "_magazine", "_projectile"];

    "chromaexecutor" callExtension "flash_white_50ms";
}];