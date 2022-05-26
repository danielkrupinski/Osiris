# Inventory Changer

## What's new
* **18 May 2022** - Pick'Em is now usable when having a tournament coin from the Inventory Changer

## Source Code
- [Backend](Backend/README.md) - simulates CSGO's item server. Implements interactions between items e.g. using a key to open a case, applying or scraping a sticker, adding a nametag etc.
- GameIntegration - is a bridge between Osiris and CSGO
- GameItems - formerly StaticData - contains the list of items in the game (skins, cases, stickers etc.) and their properties
- Inventory - defines structs describing items in the inventory
- InventoryChanger - implements applying weapon skins, stickers, music in-game
- InventoryConfig - loads/saves items from/to JSON
- ItemGenerator - implements opening weapon cases, souvenir packages and generating dynamic item properties (skin wear, seed etc.)
- StaticData - is going to be replaced by GameItems
