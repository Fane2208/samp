#if defined _systems_vehicles_included
    #endinput
#endif
#define _systems_vehicles_included

// Functie pentru incarcare masini (se apeleaza la Login)
forward LoadPlayerVehicles(playerid);
public LoadPlayerVehicles(playerid) {
    for(new i = 0; i < MAX_PLAYER_VEHICLES; i++) {
        PlayerVehicle[playerid][i][pvID] = 0;
        PlayerVehicle[playerid][i][pvSpawned] = false;
        PlayerVehicle[playerid][i][pvGameID] = 0;
    }

    new rows = cache_num_rows();
    if(rows == 0) return 1;

    for(new i = 0; i < rows && i < MAX_PLAYER_VEHICLES; i++) {
        cache_get_value_name_int(i, "id", PlayerVehicle[playerid][i][pvID]);
        cache_get_value_name_int(i, "model_id", PlayerVehicle[playerid][i][pvModel]);
        cache_get_value_name_int(i, "age", PlayerVehicle[playerid][i][pvAge]);
        cache_get_value_name_float(i, "km", PlayerVehicle[playerid][i][pvKM]);
        cache_get_value_name_float(i, "fuel", PlayerVehicle[playerid][i][pvFuel]);
        cache_get_value_name_int(i, "insurance", PlayerVehicle[playerid][i][pvInsurance]);
        cache_get_value_name_float(i, "pos_x", PlayerVehicle[playerid][i][pvPos][0]);
        cache_get_value_name_float(i, "pos_y", PlayerVehicle[playerid][i][pvPos][1]);
        cache_get_value_name_float(i, "pos_z", PlayerVehicle[playerid][i][pvPos][2]);
        cache_get_value_name_float(i, "pos_a", PlayerVehicle[playerid][i][pvPos][3]);
    }
    printf("[MySQL] Jucatorul %s a incarcat %d vehicule personale.", PlayerData[playerid][pName], rows);
    return 1;
}

YCMD:v(playerid, params[], help) {
    if(!PlayerData[playerid][pLogged]) return 1;

    new list[1024] = "Vehicul\tKM\tAge\tStatus\n", found = 0;
    for(new i = 0; i < MAX_PLAYER_VEHICLES; i++) {
        if(PlayerVehicle[playerid][i][pvID] != 0) {
            found++;
            format(list, sizeof(list), "%s%s\t%.1f\t%d\t%s\n", list, 
                GetVehicleName(PlayerVehicle[playerid][i][pvModel]),
                PlayerVehicle[playerid][i][pvKM],
                PlayerVehicle[playerid][i][pvAge],
                (PlayerVehicle[playerid][i][pvSpawned] ? ("{66FF66}Spawnat") : ("{FF3333}Despawnat")));
        }
    }

    if(found == 0) return SendClientMessage(playerid, COLOR_RED, "Nu detii niciun vehicul!");

    inline const OnVMenu(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, inputtext
        if(!response) return 1;
        
        // Deoarece dialogul arată doar mașinile pe care le ai, listitem corespunde cu slot-ul
        ShowVehicleOptions(playerid, listitem);
    }
    Dialog_ShowCallback(playerid, using inline OnVMenu, DIALOG_STYLE_TABLIST_HEADERS, "Vehiculele Tale", list, "Selecteaza", "Inchide");
    return 1;
}

stock ShowVehicleOptions(playerid, slot) {
    inline const OnOptionSelect(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, inputtext
        if(!response) return 1;

        // Folosim o variabilă locală pentru vehiculul de pe server (GameID)
        new current_vehid = PlayerVehicle[playerid][slot][pvGameID];

        switch(listitem) {
            case 0: { 
                if(!PlayerVehicle[playerid][slot][pvSpawned]) {
                    if(PlayerVehicle[playerid][slot][pvInsurance] <= 0) 
                    return SendClientMessage(playerid, COLOR_RED, "[EROARE] Masina nu are puncte de asigurare! Mergi la DS.");
                    PlayerVehicle[playerid][slot][pvGameID] = CreateVehicle(
                    PlayerVehicle[playerid][slot][pvModel], 
                    PlayerVehicle[playerid][slot][pvPos][0], 
                    PlayerVehicle[playerid][slot][pvPos][1], 
                    PlayerVehicle[playerid][slot][pvPos][2], 
                    PlayerVehicle[playerid][slot][pvPos][3], 
                    PlayerVehicle[playerid][slot][pvColor1], 
                    PlayerVehicle[playerid][slot][pvColor2], 
                    -1
                    );     
                    VehicleFuel[PlayerVehicle[playerid][slot][pvGameID]] = PlayerVehicle[playerid][slot][pvFuel];          
                    PlayerVehicle[playerid][slot][pvSpawned] = true;
                    SendClientMessage(playerid, COLOR_LIGHTPURPLE, "* Vehiculul a fost spawnat la locatia salvata.");
                } 
                else { 
                    PlayerVehicle[playerid][slot][pvFuel] = VehicleFuel[current_vehid];
                    
                    DestroyVehicle(current_vehid);
                    PlayerVehicle[playerid][slot][pvGameID] = 0;
                    PlayerVehicle[playerid][slot][pvSpawned] = false;
                    SendClientMessage(playerid, COLOR_RED, "* Vehiculul a fost despawnat.");
                }
            }
            case 1: { 
                if(!PlayerVehicle[playerid][slot][pvSpawned]) 
                    return SendClientMessage(playerid, COLOR_RED, "[EROARE] Masina trebuie sa fie spawnata!");                
                if(GetPlayerVehicleID(playerid) != current_vehid) 
                    return SendClientMessage(playerid, COLOR_RED, "[EROARE] Trebuie sa fii la volanul masinii tale!");
                GetVehiclePos(current_vehid, PlayerVehicle[playerid][slot][pvPos][0], PlayerVehicle[playerid][slot][pvPos][1], PlayerVehicle[playerid][slot][pvPos][2]);
                GetVehicleZAngle(current_vehid, PlayerVehicle[playerid][slot][pvPos][3]);
                PlayerVehicle[playerid][slot][pvFuel] = VehicleFuel[current_vehid];
                new query[300];
                mysql_format(db_handle, query, sizeof(query), 
                    "UPDATE `player_vehicles` SET `pos_x` = %f, `pos_y` = %f, `pos_z` = %f, `pos_a` = %f, `fuel` = %f, `km` = %f WHERE `id` = %d",
                    PlayerVehicle[playerid][slot][pvPos][0], PlayerVehicle[playerid][slot][pvPos][1], PlayerVehicle[playerid][slot][pvPos][2], PlayerVehicle[playerid][slot][pvPos][3],
                    PlayerVehicle[playerid][slot][pvFuel], PlayerVehicle[playerid][slot][pvKM], PlayerVehicle[playerid][slot][pvID]);
                mysql_tquery(db_handle, query);
                SendClientMessage(playerid, COLOR_LIGHTPURPLE, "* Vehiculul a fost parcat cu succes. Locatia si datele au fost salvate.");
            }
            case 2: {
                va_SendClientMessage(playerid, COLOR_WHITE, "Puncte asigurare ramase: {BF94E4}%d", PlayerVehicle[playerid][slot][pvInsurance]);
            }
        }
        return 1;
    }
    Dialog_ShowCallback(playerid, using inline OnOptionSelect, DIALOG_STYLE_LIST, "Optiuni Vehicul", "Spawn / Despawn\nParcheaza (Park)\nDetalii Asigurare", "Ok", "Inapoi");
    return 1; 
}


// --- TABEL NUME VEHICULE ---
static const VehicleNames[][] = {
    "Landstalker", "Bravura", "Buffallo", "Linerunner", "Perrenial", "Sentinel", "Dumper", "Firetruck", "Trashmaster", "Stretch", "Manana", "Infernus",
    "Voodoo", "Pony", "Mule", "Cheetah", "Ambualnce", "Leviathan", "Moonbeam", "Esperanto", "Taxi", "Washington", "Bobcat", "Mr Whoopee", "BF Injection",
    "Hunter", "Premier", "Enforcer", "Securicar", "Banshee", "Predator", "Bus", "Rhino", "Barracks", "Hotknife", "Trailer", "Previon", "Coach", "Cabbie",
    "Stallion", "Rumpo", "RC Bandit", "Romero", "Packer", "Monster", "Admiral", "Squalo", "Seasparrow", "Pizzaboy", "Tram", "Trailer", "Turismo", "Speeder",
    "Reefer", "Tropic", "Flatbed", "Yankee", "Caddy", "Solair", "Berkley's RC Van", "Skimmer", "PCJ-600", "Faggio", "Freeway", "RC Baron", "RC Raider",
    "Glendale", "Oceanic", "Sancho", "Sparrow", "Patriot", "Quad", "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350", "Walton", "Regina",
    "Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper", "Rancher", "FBI Rancher", "Virgin", "Wayfarer",
    "Utility Van", "Glendale", "Oceanic", "Sancho", "Sparrow", "Patriot", "Quad", "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350", "Walton",
    "Regina", "Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper", "Rancher", "FBI Rancher", "Virgin", "Wayfarer",
    "Utility Van", "Glendale", "Oceanic", "Sancho", "Sparrow", "Patriot", "Quad", "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR-350", "Walton",
    "Regina", "Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "News Chopper", "Rancher", "FBI Rancher", "Virgin", "Wayfarer",
    "Utility Van" // ... (poti completa lista pana la modelul 611, sau cauta o lista completa online)
};

stock GetVehicleName(modelid) {
    new name[32];
    if(modelid < 400 || modelid > 611) format(name, 32, "Necunoscut");
    else format(name, 32, "%s", VehicleNames[modelid - 400]);
    return name;
}

// --- TASK CALCUL KM (Optimizat) ---
// Rulează la fiecare 2 secunde pentru a calcula distanța parcursă
task KMTracker[2000]() {
    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;
        if(!IsPlayerInAnyVehicle(i) || GetPlayerState(i) != PLAYER_STATE_DRIVER) continue;

        new vehid = GetPlayerVehicleID(i);
        
        // Căutăm dacă mașina condusă este una personală a jucătorului
        for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
            if(PlayerVehicle[i][v][pvSpawned] && PlayerVehicle[i][v][pvGameID] == vehid) {
                // Calculăm distanța (viteză simplificată)
                new Float:vX, Float:vY, Float:vZ;
                GetVehicleVelocity(vehid, vX, vY, vZ);
                new Float:speed = (VectorSize(vX, vY, vZ) * 100.0) / 50.0; // Distanța parcursă în 2 secunde aprox.
                
                PlayerVehicle[i][v][pvKM] += (speed / 100.0); // Adăugăm la kilometraj
                break;
            }
        }
    }
}

// --- ACTUALIZARE AGE (În PayDay) ---
// Adaugă asta în payday.inc sau aici sub formă de hook
hook TriggerPayDay() {
    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;
        for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
            if(PlayerVehicle[i][v][pvID] != 0) {
                // Creștem vârsta mașinii la fiecare PayDay (sau poți face la ora 00:00)
                // Dacă vrei Age în zile reale, verificăm dacă e ora 00:00
                new h, m, s;
                gettime(h, m, s);
                if(h == 0 && m == 0) PlayerVehicle[i][v][pvAge]++;
            }
        }
    }
    return 1;
}

// --- ASIGURARE (OnVehicleDeath) ---
hook OnVehicleDeath(vehicleid, killerid) {
    foreach(new i : Player) {
        if(!PlayerData[i][pLogged]) continue;
        for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
            if(PlayerVehicle[i][v][pvSpawned] && PlayerVehicle[i][v][pvGameID] == vehicleid) {
                PlayerVehicle[i][v][pvSpawned] = false;
                PlayerVehicle[i][v][pvGameID] = 0;
                PlayerVehicle[i][v][pvInsurance]--;
                
                va_SendClientMessage(i, COLOR_RED, "* Masina ta (%s) a fost distrusa. Puncte asigurare ramase: %d.", 
                    GetVehicleName(PlayerVehicle[i][v][pvModel]), PlayerVehicle[i][v][pvInsurance]);
                
                if(PlayerVehicle[i][v][pvInsurance] <= 0) {
                    SendClientMessage(i, COLOR_RED, "Masina nu mai are asigurare! Mergi la dealership sa cumperi.");
                }
                
                // Salvare automată a noii valori de asigurare
                new query[128];
                mysql_format(db_handle, query, sizeof(query), "UPDATE `player_vehicles` SET `insurance` = %d WHERE `id` = %d", 
                    PlayerVehicle[i][v][pvInsurance], PlayerVehicle[i][v][pvID]);
                mysql_tquery(db_handle, query);
                break;
            }
        }
    }
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate) {
    // Verificăm dacă jucătorul a intrat într-un vehicul (ca șofer sau pasager)
    if(newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) {
        new vehid = GetPlayerVehicleID(playerid);
        
        // Căutăm proprietarul acestui vehicul în lista de jucători online
        foreach(new i : Player) {
            if(!PlayerData[i][pLogged]) continue;

            for(new v = 0; v < MAX_PLAYER_VEHICLES; v++) {
                // Dacă găsim mașina în sloturile de vehicule spawnate ale unui jucător
                if(PlayerVehicle[i][v][pvSpawned] && PlayerVehicle[i][v][pvGameID] == vehid) {
                    
                    new infoStr[256];
                    // Formatăm partea cu informații (Age, KM, Colors) care este comună
                    format(infoStr, sizeof(infoStr), "(Age: {BF94E4}%d {FFFFFF}| KM: {BF94E4}%.1f {FFFFFF}| Colors: {BF94E4}%d, %d{FFFFFF})",
                        PlayerVehicle[i][v][pvAge], 
                        PlayerVehicle[i][v][pvKM], 
                        PlayerVehicle[i][v][pvColor1], 
                        PlayerVehicle[i][v][pvColor2]
                    );

                    if(i == playerid) {
                        // Mesaj pentru propria mașină
                        va_SendClientMessage(playerid, COLOR_WHITE, "Te-ai urcat in masina ta personala {BF94E4}%s {FFFFFF}%s.", 
                            GetVehicleName(PlayerVehicle[i][v][pvModel]), infoStr);
                    }
                    else {
                        // Mesaj pentru mașina altcuiva
                        va_SendClientMessage(playerid, COLOR_WHITE, "Te-ai urcat in masina lui {BF94E4}%s {FFFFFF}%s.",
                            PlayerData[i][pName], infoStr);
                    }
                    return 1; 
                }
            }
        }
    }
    return 1;
}