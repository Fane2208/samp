#if defined _systems_dealership_included
    #endinput
#endif
#define _systems_dealership_included

#include <YSI_Coding\y_hooks>

#define MAX_DS_CARS 50
enum E_DS_CATALOG {
    dsID, dsModel, dsName[32], dsPrice, dsStock
};
new DSCatalog[MAX_DS_CARS][E_DS_CATALOG], TotalDSCars = 0;

hook OnGameModeInit() {
    mysql_tquery(db_handle, "SELECT * FROM `ds_catalog` ORDER BY `price` ASC", "LoadDSCatalog");
    return 1;
}

forward LoadDSCatalog();
public LoadDSCatalog() {
    TotalDSCars = cache_num_rows();
    for(new i = 0; i < TotalDSCars && i < MAX_DS_CARS; i++) {
        cache_get_value_name_int(i, "id", DSCatalog[i][dsID]);
        cache_get_value_name_int(i, "model_id", DSCatalog[i][dsModel]);
        cache_get_value_name_int(i, "price", DSCatalog[i][dsPrice]);
        cache_get_value_name_int(i, "stock", DSCatalog[i][dsStock]);
        cache_get_value_name(i, "name", DSCatalog[i][dsName]);
    }
    printf("[MySQL] Catalog Dealership incarcat: %d masini.", TotalDSCars);
}

YCMD:buyvehicle(playerid, params[], help) {
    if(PlayerData[playerid][pLevel] < 3) return SendClientMessage(playerid, COLOR_RED, "Ai nevoie de minim level 3 pentru a cumpara o masina!");
    if(!IsPlayerInRangeOfPoint(playerid, 10.0, 2022.3240, 1007.7017, 10.8203)) return SendClientMessage(playerid, COLOR_RED, "Trebuie sa fii la Dealership (Grotti)!");

    new list[1024] = "Model\tPret\tStoc\n";
    for(new i = 0; i < TotalDSCars; i++) {
        format(list, sizeof(list), "%s%s\t" C_LIGHTPURPLE "$%s\t" C_WHITE "%d\n", list, DSCatalog[i][dsName], va_return("%d", DSCatalog[i][dsPrice]), DSCatalog[i][dsStock]);
    }

    inline const OnDSSelect(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, inputtext
        if(!response) return 1;
        
        if(DSCatalog[listitem][dsStock] <= 0) return SendClientMessage(playerid, COLOR_RED, "Acest model nu mai este in stoc!");
        if(PlayerData[playerid][pMoney] < DSCatalog[listitem][dsPrice]) return SendClientMessage(playerid, COLOR_RED, "Nu ai suficienti bani!");

        // Verificare sloturi (Simplificat: trimitem un query sa numaram masinile din DB)
        new query[256];
        mysql_format(db_handle, query, sizeof(query), "SELECT id FROM player_vehicles WHERE owner_id = %d", PlayerData[playerid][pID]);
        mysql_tquery(db_handle, query, "OnCheckSlotsBeforeBuy", "dd", playerid, listitem);
    }

    Dialog_ShowCallback(playerid, using inline OnDSSelect, DIALOG_STYLE_TABLIST_HEADERS, "Dealership", list, "Cumpara", "Iesire");
    return 1;
}

forward OnCheckSlotsBeforeBuy(playerid, ds_index);
public OnCheckSlotsBeforeBuy(playerid, ds_index) {
    if(cache_num_rows() >= 5) return SendClientMessage(playerid, COLOR_RED, "Ai atins limita maxima de 5 vehicule!");

    // Totul e OK - Scadem banii si stock-ul
    GivePlayerCash(playerid, -DSCatalog[ds_index][dsPrice]);
    DSCatalog[ds_index][dsStock]--;
    
    new query[400];
    mysql_format(db_handle, query, sizeof(query), "UPDATE ds_catalog SET stock = stock - 1 WHERE id = %d", DSCatalog[ds_index][dsID]);
    mysql_tquery(db_handle, query);

    // Grotti Spawn Pos
    mysql_format(db_handle, query, sizeof(query), 
        "INSERT INTO player_vehicles (owner_id, model_id, price, pos_x, pos_y, pos_z, pos_a) VALUES (%d, %d, %d, 2022.3240, 1007.7017, 10.8203, 0.0)", 
        PlayerData[playerid][pID], DSCatalog[ds_index][dsModel], DSCatalog[ds_index][dsPrice]);
    
    mysql_tquery(db_handle, query, "OnVehiclePurchased", "d", playerid);
    return 1;
}

forward OnVehiclePurchased(playerid);
public OnVehiclePurchased(playerid) {
    SendClientMessage(playerid, COLOR_LIGHTPURPLE, "Felicitari! Ai cumparat o masina noua. O poti gestiona folosind comanda /v.");
    new query[128];
    mysql_format(db_handle, query, sizeof(query), "SELECT * FROM `player_vehicles` WHERE `owner_id` = %d LIMIT %d", PlayerData[playerid][pID], MAX_PLAYER_VEHICLES);
    mysql_tquery(db_handle, query, "LoadPlayerVehicles", "d", playerid);
    return 1;
}