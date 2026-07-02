class CfgPatches {
    class a3chroma_addons {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {};
    };
};

class CfgFunctions {
    class a3chroma {
        class init {
            class initPlayerLocal {
                file = "\a3chroma\addons\initPlayerLocal.sqf";
                preInit = 1;
            };
        };
    };
};
