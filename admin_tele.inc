#if defined _systems_admin_tele_included
    #endinput
#endif
#define _systems_admin_tele_included

#include <YSI_Coding\y_hooks>

hook OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ) {
    // Verificăm dacă este admin (minim level 1)
    if(PlayerData[playerid][pAdmin] < 1) return 0;

    // Resetăm interiorul și virtual world-ul pentru a nu rămâne blocat într-o casă
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);

    // Înghețăm jucătorul pentru a permite hărții să se încarce
    TogglePlayerControllable(playerid, false);

    if(IsPlayerInAnyVehicle(playerid)) {
        new vehid = GetPlayerVehicleID(playerid);
        
        // Teleportăm vehiculul puțin mai sus ca să nu se blocheze în asfalt
        SetVehiclePos(vehid, fX, fY, fZ + 3.0);
        
        // FindZ pentru vehicule nu este nativ perfect, așa că folosim o mică întârziere
        defer UnfreezeAdmin(playerid, fX, fY);
    } 
    else {
        // Teleportăm jucătorul la coordonatele X, Y și o înălțime de siguranță
        SetPlayerPos(playerid, fX, fY, fZ + 3.0);
        
        // Folosim funcția nativă pentru a găsi solul (Z)
        SetPlayerPosFindZ(playerid, fX, fY, fZ + 100.0);
        
        defer UnfreezeAdmin(playerid, fX, fY);
    }

    va_SendClientMessage(playerid, COLOR_LIGHTPURPLE, "[TELEPORT] {FFFFFF}Te-ai teleportat la locatia selectata pe harta (X: %.2f | Y: %.2f).", fX, fY);
    PlayerPlaySound(playerid, 1085, 0.0, 0.0, 0.0); // Sunet de confirmare
    return 1;
}

// Timer pentru dezghețare (oferă timp obiectelor să se încarce)
timer UnfreezeAdmin[500](playerid, Float:x, Float:y) {
    TogglePlayerControllable(playerid, true);
    
    // Verificăm dacă jucătorul este în vehicul
    if(IsPlayerInAnyVehicle(playerid)) {
        // Dacă e în mașină, nu mai e nevoie de FindZ suplimentar (mașina are coliziuni proprii)
        return 1;
    }
    
    // Încercăm din nou să găsim solul pentru jucătorul pe picioare
    SetPlayerPosFindZ(playerid, x, y, 1000.0);
    return 1;
}