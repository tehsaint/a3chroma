// Simple Chroma test: when the player fires a weapon, flash the keyboard white for 50 ms.
//
// This is MEANT to do two simply things:
// 1. On startup, initialize the Chroma extension.
// 2. On a fire event, trigger a short white flash on your keyboard
// then boom, success MAYBE. huzzah

private _result = "chroma_rest_bridge" callExtension "init";
diag_log format ["[a3chroma] init result: %1", _result];

if (isServer) then {
    diag_log "[a3chroma] init executed on server";
};

if (hasInterface) then {
    diag_log "[a3chroma] init executed on client";
};

if (_result == "ok") then {
    hint "Chroma initialized successfully!";
} else {
    hint format ["Chroma init failed: %1", _result];
};

player addEventHandler ["Fired", {
    params ["_unit", "_weapon", "_muzzle", "_mode", "_ammo", "_magazine", "_projectile"];

    private _fireResult = "chroma_rest_bridge" callExtension "flash_white_50ms";
    diag_log format ["[a3chroma] fire result: %1", _fireResult];
    diag_log "[a3chroma] Fired event triggered";

    if (_fireResult != "ok") then {
        systemChat format ["[a3chroma] fire failed: %1", _fireResult];
    };
}];