#if defined _systems_world_sync_included
    #endinput
#endif
#define _systems_world_sync_included

#include <YSI_Coding\y_hooks>

static g_CurrentWeather = 1;

hook OnGameModeInit() {
    // Setăm ora și vremea inițială la pornirea serverului
    UpdateWorldEnvironment();
    return 1;
}

// Task care rulează la fiecare 60 de secunde (optimizat)
task WorldSyncTimer[60000]() {
    UpdateWorldEnvironment();
}

stock UpdateWorldEnvironment() {
    new h, m, s;
    gettime(h, m, s);

    // Sincronizăm ora lumii cu ora reală
    // SetWorldTime setează poziția soarelui/lunii pentru toți jucătorii
    SetWorldTime(h);

    // --- LOGICĂ DE VREME DINAMICĂ (Clean) ---
    // Putem seta vremea în funcție de intervale orare pentru realism
    new new_weather;

    switch(h) {
        case 0..5:   new_weather = 1;  // Noapte senină
        case 6..10:  new_weather = 10; // Dimineață însorită
        case 11..17: new_weather = 11; // Amiază (fără nori)
        case 18..20: new_weather = 10; // Apus
        case 21..23: new_weather = 1;  // Seară
    }

    // Actualizăm vremea global doar dacă s-a schimbat (optimizare)
    if(new_weather != g_CurrentWeather) {
        g_CurrentWeather = new_weather;
        SetWeather(g_CurrentWeather);
        printf("[SERVER] Vremea s-a schimbat automat la ID %d (Ora: %d:00)", g_CurrentWeather, h);
    }
}

// Asigurăm sincronizarea când un jucător se conectează
hook OnPlayerConnect(playerid) {
    new h, m, s;
    gettime(h, m, s);
    SetPlayerTime(playerid, h, m);
    return 1;
}