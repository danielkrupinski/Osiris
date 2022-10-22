# Inventory Changer

## What's new

* **17 August 2022** - Crate keys (and items unlocked using them) now appear as untradable for one week when added to the inventory.
* **4 August 2022** - IEM Katowice 2019 souvenirs now receive teams / MVP gold stickers
* **1 August 2022** - Storage Units can be used to store items from the Inventory Changer
* **14 June 2022** - X-Ray Scanner can be used to open weapons cases from the Inventory Changer
* **11 June 2022** - Added "Add all in list" button to GUI which adds all items satisfying filter condition to the inventory.
Example: searching for "glock-18" and pressing "Add all in list" will add all weapon skins for Glock-18.
* **18 May 2022** - Pick'Em is now usable when having a tournament coin from the Inventory Changer

## Features

* Weapon skins, knives
  * StatTrak counts kills
* Gloves, agents
* Agent patches
  * can be applied to agents and removed from them
* Stickers
  * can be applied to weapons and scraped
* Weapon cases, sticker capsules, pins capsules, graffiti boxes, music kit boxes
  * can be opened (also with X-Ray Scanner)
  * item drop rates resemble real drop rates
  * "special rare items" can be unboxed
  * StatTrak items can be unboxed
* Souvenir packages
  * can be opened
  * have real teams and MVP player from a random match
  * unboxed skins get gold stickers and teams / MVP in their description
    * excluding **StarLadder Berlin 2019** tournament, which is yet to be implemented
* Music kits
  * in main menu and in game
  * StatTrak counts round MVPs
* Operation passes
  * can be activated
  * give bronze operation coin when activated
* Tournament passes
  * can be activated
  * give bronze tournament coin when activated
* Tournament coins
  * team graffiti can be selected
* Name tag
  * can be used on skins and later removed
* StatTrak swap tool
  * can be used to swap StatTrak values between weapon skins
* Service medals and collectible pins
  * are displayed next to the avatar and on the in-game scoreboard when equipped
* Storage unit
  * can be renamed
  * items can be stored/retrieved

and more...

## Source Code

* [Backend](Backend/README.md) - simulates CSGO's item server. Implements interactions between items e.g. using a key to open a case, applying or scraping a sticker, adding a nametag etc.
* GameIntegration - is a bridge between Osiris and CSGO
* GameItems - contains the list of items in the game (skins, cases, stickers etc.) and their properties
* Inventory - defines structs describing items in the inventory
* InventoryChanger - implements applying weapon skins, stickers, music in-game
* InventoryConfig - loads/saves items from/to JSON
* ItemGenerator - implements opening weapon cases, souvenir packages and generating dynamic item properties (skin wear, seed etc.)
