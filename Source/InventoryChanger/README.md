# Inventory Changer

## Source Code
- GameItems - formerly StaticData - contains the list of items in the game (skins, cases, stickers etc.) and their properties
- Inventory - stores items added to the inventory
- InventoryChanger - implements applying weapon skins, stickers, music in-game
- InventoryConfig - loads/saves items from/to JSON
- ItemGenerator - implements opening weapon cases, souvenir packages and generating dynamic item properties (skin wear, seed etc.)
- StaticData - is going to be replaced by GameItems
- ToolUser - implements interactions between items e.g. using a key to open a case, applying or scraping a sticker, adding a nametag etc.
- TournamentMatches.h - contains information used to generate stickers on souvenir skins
