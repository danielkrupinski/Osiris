# Osiris 
[![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=plastic)](https://en.wikipedia.org/wiki/C%2B%2B) 
[![CS:GO](https://img.shields.io/badge/game-CS%3AGO-yellow.svg?style=plastic)](https://store.steampowered.com/app/730/CounterStrike_Global_Offensive/) 
[![License](https://img.shields.io/github/license/danielkrupinski/Osiris.svg?style=plastic)](LICENSE)
[![Issues](https://img.shields.io/github/issues/danielkrupinski/Osiris.svg?style=plastic)](https://github.com/danielkrupinski/Osiris/issues)
[![PayPal](https://img.shields.io/badge/donate-PayPal-104098.svg?style=plastic&logo=PayPal)](https://paypal.me/DanielK19)
<br>![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)
![Linux](https://github.com/danielkrupinski/Osiris/workflows/Linux/badge.svg?branch=master&event=push)

适用于 **反恐精英: 全球攻势** 的免费、开源、跨平台的作弊软件。设计为内部作弊 - [动态链接库](https://en.wikipedia.org/wiki/Dynamic-link_library) (DLL) 可加载到游戏进程中。与 Steam 版本的游戏兼容。适用于 Windows 和 Linux 系统。

如果你需要反馈任何并非由翻译造成的问题，请前往 [Osiris](https://github.com/danielkrupinski/Osiris) 仓库提交 Issue。

## 特性
*   **自瞄** - 瞄准辅助
*   **开火** - 当准心瞄准敌人时自动开火
*   **回溯** - 利用延迟补偿以使玩家及时回退
*   **描边** - 在实体上渲染光晕效果
*   **上色** - 给玩家模型上色，以提高可见性
*   **ESP** - 显示玩家、投掷武器和射弹的信息
*   **视觉** - 其他视觉选项
*   [**物品替换**](Source/InventoryChanger/) - 将武器皮肤、贴纸、刀、手套、奖牌等添加到游戏的库存中
*   **声音** - 修改某些声音效果的音量
*   **主题** - 选择菜单窗口布局和颜色
*   **其他** - 杂项功能
*   **配置** - 基于 JSON 的配置系统

<details>

*   **自瞄** - aim assistance
    *   **启用** - 主开关
    *   **On key \[ key \]** - 自瞄仅在选中的键按下时工作
    *   **Aimlock** - brings your aim to the target (affected by Smooth).
    *   **Silent** - aimbot is not visible on your screen (client-sided only)
    *   **Friendly fire** - treat allies as enemies
    *   **Visible only** - aim only on visible players
    *   **Scoped only** - aimbot works only when using scope (applies only to sniper rifles)
    *   **Ignore flash** - ignore flashbang i.e. aim when local player is flashed
    *   **Ignore smoke** - ignore smoke i.e. aim when target is in smoke
    *   **Auto shot** - shoot automatically when target found
    *   **Auto scope** - automatically scopes sniper rifle before shooting
    *   **Bone** - bone which aimbot aims at
    *   **Fov** - field-of-view which aimbot operates \[*0*-*255*\]
    *   **Smooth** - smooth aimbot movement in order to seem more human-like
    *   **Max aim inaccuracy** - maximum weapon inaccuracy allowing aimbot to run, lowering this value will e.g. disable aimbot while jumping or running

*   **开火** - automatically fires when crosshair is on enemy
    *   **启用** - 主开关
    *   **On key \[ key \]** - triggerbot works only when chosen key is being held
    *   **Friendly fire** - treat allies as enemies
    *   **Scoped only** - triggerbot works only when using scope (applies only to sniper rifles)
    *   **Ignore flash** - ignore flashbang i.e. shoot when local player is flashed
    *   **Ignore smoke** - ignore smoke i.e. shoot when target is in smoke
    *   **Hitgroup** - body parts on which triggerbot works
    *   **Shot delay** - delay time in ms (milliseconds)
    *   **Min damage** - minimal damage to fire.

*   **回溯** - abuse lag compensation in order to move players back in time
    *   **启用** - 主开关
    *   **Ignore smoke** - ignore smoke i.e. backtrack when target is in smoke
    *   **Time limit** - limit the backtracking window \[*1*-*200*ms\]

*   **描边** - render glow effect on entities

    *Allies, Enemies, Planting (player planting bomb), Defusing (player defusing bomb), Local player, Weapons (dropped weapons), C4, Planted C4, Chickens, Defuse kits, Projectiles, Hostages, Ragdolls* **/** *All, Visible, Occluded*

    *   **启用** - 主开关
    *   **Health based** - color is based on player's hp
    *   **Color** - glow color in rgba format
    *   **Style** - glow style { `Default`, `Rim3d`, `Edge`, `Edge Pulse` }

*   **上色** - color player models to improve visibility

    *Allies, Enemies, Planting (player planting bomb), Defusing (player defusing bomb), Local player, Weapons (dropped weapons), Hands (view model hands), Backtrack (requires backtrack to be enabled), Sleeves (view model)* **/** *All, Visible, Occluded*
    *   **启用** - 主开关
    *   **Health based** - color is based on player's hp
    *   **Blinking** - change transparency frequently
    *   **Material** - material applied to model { `Normal`, `Flat`, `Animated`, `Platinum`, `Glass`, `Chrome`, `Crystal`, `Silver`, `Gold`, `Plastic`, `Glow` }
    *   **Wireframe** - render triangle mesh instead of solid material
    *   **Cover** - draw chams material on top of the original material instead of overriding it
    *   **Ignore-Z** - draw material through walls

*   **ESP** - show additional information about players and game world
    1.  *Allies, Enemies*
        *   *All, Visible, Occluded*

    2.  *Weapons*

    3.  *Projectiles*
        *   *Flashbang, HE Grenade, Breach Charge, Bump Mine, Decoy Grenade, Molotov, TA Grenade, Smoke Grenade, Snowball*

    4.  *Danger Zone*
        *   *Sentries, Drones, Cash, Cash Dufflebag, Pistol Case, Light Case, Heavy Case, Explosive Case, Tools Case, Full Armor, Armor, Helmet, Parachute, Briefcase, Tablet Upgrade, ExoJump, Ammobox, Radar Jammer*

    *   **启用** - 主开关
    *   **Font** - esp text font
    *   **Snaplines** - draw snapline to player
    *   **Eye traces** - draw player eye traces (shows where player looks)
    *   **Box** - draw 2D box over player model
    *   **Name** - draw player name
    *   **Health** - draw player health
    *   **Health bar** - draw rectangle indicating player health
    *   **Armor** - draw player armor
    *   **Armor bar** - draw rectangle indicating player armor
    *   **Money** - draw player money
    *   **Head dot** - draw dot on player's head
    *   **Active Weapon** - draw player equipped weapon

*   **视觉** - miscellaneous visual options
    *   **Disable post-processing** - disable post-processing effects in order to increase FPS
    *   **Inverse ragdoll gravity** - inverse gravitational acceleration on falling player ragdoll corpse (during death sequence)
    *   **No fog** - remove fog from map for better visibility
    *   **No 3d sky** - remove 3d skybox from map - increases FPS
    *   **No visual recoil** - remove visual recoil punch effect
    *   **No hands** - remove arms / hands model from first-person view
    *   **No sleeves** - remove sleeves model from first-person view
    *   **No weapons** - remove weapons model from first-person view
    *   **No smoke** - remove smoke grenade effect
    *   **No blur** - remove blur
    *   **No scope overlay** - remove black overlay while scoping
    *   **No grass** - remove grass from map in Danger Zone mode (`dz_blacksite` and `dz_sirocco` maps)
    *   **No shadows** - disable dynamic shadows
    *   **Wireframe smoke** - render smoke skeleton instead of particle effect
    *   **Zoom \[ key \]** - enable zoom on unzoomable weapons
    *   **Thirdperson** - thirdperson view
    *   **Thirdperson distance** - camera distance in thirdperson view
    *   **View model FOV** - change view model FOV \[*-60*-*0*-*60*\] (0 - actual view model, negative values - decreased view model, positive values - increased view model)
    *   **FOV** - change view FOV \[*-60*-*0*-*60*\] (0 - actual view fov, negative values - decreased, positive values - increased)
    *   **Far Z** - far clipping range, useful after disabling fog on large maps (e.g `dz_sirocco`) to render distant buildings
    *   **Flash reduction** - reduces flashbang grenade effect \[*0*-*100*%\] (0 - full flash, 100 - no flash)
    *   **Brightness** - control game brightness \[*0.0*-*1.0*\]
    *   **Skybox** - change sky(box)
    *   **World color** - set world material ambient light color
    *   **Deagle spinner** - play "spinning" inspect animation when holding Deagle
    *   **Screen effect** - screenspace effect - *Drone cam, Drone cam with noise, Underwater, Healthboost, Dangerzone*
    *   **Hit effect** - show screen effect on enemy hit
    *   **Hit marker** - show a cross detail on enemy hit

*   **物品替换** - add weapon skins, stickers, knives, gloves, medals and more to your in-game inventory

*   **声音** - modify volume of certain sound effects
    *   **Chicken volume** - volume of chicken sounds

    *Local player, Allies, Enemies*
    *   **Master volume** - overall volume of sounds emitted by player
    *   **Headshot volume** - volume of headshot sound (when player gets headshoted)
    *   **Weapon volume** - volume of player weapon shots
    *   **Footstep volume** - volume of player footsteps

*   **其他** - miscellaneous features
    *   **Menu key \[ key \]** - menu toggle key

    *   **Menu style** - menu style toggle (*Classic* **/** *One window*)

    *   **Menu colors** - menu color theme (*Dark **/** Light **/** Classic*)

    *   **Anti AFK kick** - avoid auto-kick by server for inactivity

    *   **Auto strafe** - automatically strafe in air following mouse movement

    *   **Bunny hop** - automatically simulate space bar press / release while jump button is being held; increases movement speed

    *   **Clan tag** - set custom clan tag

    *   **Animated clan tag** - animate clan tag

    *   **Fast duck** - remove crouch delay

    *   **Sniper crosshair** - draw crosshair while holding sniper rifle

    *   **Recoil crosshair** - crosshair follows recoil pattern

    *   **Auto pistol** - fire pistols like automatic rifles

    *   **Auto reload** - automatically reload if weapon has empty clip

    *   **Auto accept** - automatically accept competitive match

    *   **Radar hack** - show enemies positions on radar

    *   **Reveal ranks** - show player ranks in scoreboard in competitive modes

    *   **Reveal money** - show enemies' money in scoreboard

    *   **Spectator list** - show nicknames of players spectating you

    *   **Watermark** - show cheat name in the upper-left screen corner and fps & ping in the upper-right corner

    *   **Offscreen Enemies** - draw circles on the screen indicating that there are enemies behind us

    *   **Fix animation LOD** - fix aimbot inaccuracy for players behind local player

    *   **Fix bone matrix** - correct client bone matrix to be closer to server one

    *   **Disable model occlusion** - draw player models even if they are behind thick walls

    *   **Kill message** - print message to chat after killing an enemy

    *   **Name stealer** - mimic other players names

    *   **Custom clantag** - set a custom clantag

    *   **Fast plant** - plants bomb on bombsite border, when holding <kbd>LMB</kbd> or <kbd>E</kbd> key

    *   **Fast Stop** - stops the player faster than normal

    *   **Quick reload** - perform quick weapon switch during reload for faster reload

    *   **Prepare revolver \[ key \]** - keep revolver cocked, optionally on key

    *   **Fix tablet signal** - allow use tablet on underground (dangerzone)

    *   **Hit Sound** - sound emitted when hurting enemy

    *   **Chocked packets** - length of sequence of chocked ticks

    *   **Max angle delta** - maximum viewangles change per tick

    *   **Aspect Ratio** - allows you to change the aspect ratio

    *   **Purchase List** - show the purchased equipment by enemies.

    *   **Reportbot** - automatically report players on server for cheating or other abusive actions
        *   **启用** - 主开关
        *   **Target** - report target *Enemies/Allies/All*
        *   **Delay** - delay between reports, in seconds
        *   **Aimbot** - report for aim assistance
        *   **Wallhack** - report for visual assistance
        *   **Other** - report for other assistance
        *   **Griefing** - report for griefing
        *   **Abusive Communications** - report for abusive communications

    *   **Unhook** - unload cheat

*   **Config** - JSON-based configuration system
    *   **Create config** - create new configuration file
    *   **Reset config** - restore default configuration settings (does not touch saved configuration)
    *   **Load selected** - load selected configuration file
    *   **Save selected** - save selected configuration file
    *   **Delete selected** - delete selected configuration file
    *   **Reload configs** - reload configs list
</details>

## 入门

### 编译环境
Microsoft Visual Studio 2019 16.10 (或更新版本), 平台工具集 v142 和 Windows SDK 10.0.x.x。如果你没有, 可以在 [这里](https://visualstudio.microsoft.com/) 下载 Visual Studio (安装过程中将会安装 Windows SDK)。

### 获取源代码

获取源代码有两种选择：

#### 没有 [git](https://git-scm.com)

如果你只需要源代码，并且你不打算参与开发，请选择此选项。下载大小约为 600 KB。

点击 [此处](https://github.com/nkxingxh/Osiris/archive/master.zip) 下载源代码。

#### 使用 [git](https://git-scm.com)

如果你打算参与开发或者想要使用版本控制系统，请选择此选项。下载大小约为 4 MB。没安装 git 或不会用的自行百度。

打开 git 命令行并输入以下命令: 

    git clone --depth=1 https://github.com/nkxingxh/Osiris.git

`Osiris` 文件夹应该已成功创建，包含所有源文件。

### 从源代码编译

当您获取了源代码副本后，下一步是在 Microsoft Visual Studio 2019 中打开 **Osiris.sln**。

然后将构建配置更改为 `Release | x86`并点击 **生成解决方案** 即可。

如果一切顺利，你应该得到 `Osiris.dll` 二进制文件。

### 加载 / 注入游戏进程

打开你喜欢的 [DLL 注入器](https://en.wikipedia.org/wiki/DLL_injection) 并将 `Osiris.dll` 注入进 `csgo.exe` 进程.

注入后，可通过 `INSERT` 键打开菜单。

### 进一步优化
如果您的 CPU 支持 AVX / AVX2 / AVX-512 指令集，则可以在项目设置中启用它。这将产生更高性能的代码，并针对您的 CPU 进行优化。当前在项目设置中选择 SSE2 指令。

## FAQ

### 如何打开菜单？
当焦点在游戏窗口时按 <kbd>INSERT</kbd> 键。

### 我的配置文件保存在哪里？
配置文件保存在你账户的 `文档` 文件夹 (`%USERPROFILE%\Documents\Osiris`)。配置文件为可读的格式，并且你可以编辑。有时在更新后，需要删除并重新创建配置文件。

### Osiris 使用什么 hook 方法？
当前使用的 hook 方法有:
*   MinHook - trampoline hook
*   VmtHook - hook a function directly in a vtable
*   VmtSwap - create a copy of a vtable and swap the pointer on the class instance

hook 实现文件位于 [Hooks](Source/Hooks) 目录。

## 致谢

*   [ocornut](https://github.com/ocornut) 与 [contributors](https://github.com/ocornut/imgui/graphs/contributors) 创建并维护一个惊人的 GUI 库 - [Dear imgui](https://github.com/ocornut/imgui).
*   [Zer0Mem0ry](https://github.com/Zer0Mem0ry) - 关于逆向工程和游戏外挂的优秀教程

## 许可

> Copyright (c) 2018-2022 Daniel Krupiński

This project is licensed under the [MIT License](https://opensource.org/licenses/mit-license.php) - see the [LICENSE](https://github.com/danielkrupinski/Osiris/blob/master/LICENSE) file for details.

## 另请参见
*   [Anubis](https://github.com/danielkrupinski/Anubis) - 为 反恐精英:全球攻势 设计的免费开源作弊程序
*   [GOESP](https://github.com/danielkrupinski/GOESP) - 适用于 反恐精英:全球攻势 的免费开源跨平台的 streamproof ESP 工具，使用现代 C++ 编写
