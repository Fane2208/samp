#pragma warning disable 239
#define YSI_NO_HEAP_MALLOC
#include <a_samp>
#include <a_mysql>
#include <sscanf2>

// --- LIBRARII YSI ---
#include <YSI_Coding\y_va>
#include <YSI_Coding\y_timers>
#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_inline>
#include <YSI_Visual\y_dialog>
#include <YSI_Data\y_foreach>
#include <YSI_Visual\y_commands>

// --- MODULE CORE (SISTEME) ---
#include <systems/colors>
#include <systems/config>
#include <systems/database>
#include <systems/player_main>
#include <systems/admin>
#include <systems/auth>
#include <systems/player_cmds>
#include <systems/spawn>
#include <systems/textdraws>
#include <systems/server_mgmt>
#include <systems/chat.inc>
#include <systems/payday>
#include <systems/environment>
#include <systems/world_sync>
#include <systems/dealership>
#include <systems/vehicles_main>
#include <systems/enginelights.inc>
#include <systems/fuel>
#include <systems/admin_tele>
#include <systems/speedometer>

main() {
    print("\n-----------------------------------------");
    print(" RPG.OLD-PLAY.RO");
    print(" v0.1");
    print("-----------------------------------------\n");
}