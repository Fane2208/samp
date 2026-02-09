#if defined _systems_environment_included
    #endinput
#endif
#define _systems_environment_included

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {
    // 1. Dezactivăm intrările/ieșirile automate (săgețile galbene din fața clădirilor)
    // Acum jucătorii nu mai pot intra în clădirile default din Single Player
    DisableInteriorEnterExits();

    // 2. Dezactivăm bonusul de bani pentru cascadorii (Stunt Bonus)
    EnableStuntBonusForAll(false);

    // 3. Dezactivăm markerii jucătorilor pe mapă (RPG Style - nu îi vezi pe hartă)
    ShowPlayerMarkers(PLAYER_MARKERS_MODE_OFF);

    // 4. Setăm distanța de afișare a numelui (NameTag)
    SetNameTagDrawDistance(20.0);

    // 5. Activăm controlul manual pentru motor și lumini (foarte important pentru RPG)
    // Fără asta, mașinile pornesc singure când intri în ele.
    ManualVehicleEngineAndLights();

    print("[SERVER] Mediul RPG a fost configurat (Intrari dezactivate, Stunt Off, Manual Engine On).");
    return 1;
}

hook OnPlayerConnect(playerid) {
    // Dezactivăm ceasul de bază al SA-MP (cel din colțul dreapta sus)
    // pentru că am făcut noi unul personalizat în textdraws.inc
    TogglePlayerClock(playerid, false);
    return 1;
}