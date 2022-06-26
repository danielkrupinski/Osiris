# Inventory Changer

## What's new
* **14 June 2022** - X-Ray Scanner can be used to open weapons cases from the Inventory Changer
* **11 June 2022** - Added "Add all in list" button to GUI which adds all items satisfying filter condition to the inventory.
Example: searching for "glock-18" and pressing "Add all in list" will add all weapon skins for Glock-18.
* **18 May 2022** - Pick'Em is now usable when having a tournament coin from the Inventory Changer

## Source Code
- [Backend](Backend/README.md) - simulates CSGO's item server. Implements interactions between items e.g. using a key to open a case, applying or scraping a sticker, adding a nametag etc.
- GameIntegration - is a bridge between Osiris and CSGO
- GameItems - contains the list of items in the game (skins, cases, stickers etc.) and their properties
- Inventory - defines structs describing items in the inventory
- InventoryChanger - implements applying weapon skins, stickers, music in-game
- InventoryConfig - loads/saves items from/to JSON
- ItemGenerator - implements opening weapon cases, souvenir packages and generating dynamic item properties (skin wear, seed etc.)
