## Individual XP Module

This module allows each player to `view`, `set`, and `modify` their experience multiplier without having to modify the values of other players. This functionality can be accessed at any time through the use of commands. It is the player who decides how to play (`slow` or `fast`), rather than the server forcing them to decide for them.

# How to install

1. Clone this module into the modules directory of the main source
2. Re-run cmake
3. Compile.

# Config

There are two variables to configure in the Config:

- `IndividualXp.Enabled`
    - Enable or Disable the IndividualXP Module.
- `IndividualXp.Announce`
    - Announce the IndividualXP Module at logon.
- `IndividualXp.AnnounceRatesOnLogin`
    - The player is shown the rates he has and the maximum value when he enters the chat.
- `IndividualXp.MaxXPRate`
    - This is the max amount a player can set their xp to.
- `IndividualXp.DefaultXPRate`
    - This is the default rate players start with.

The Max XP Rate variable dictates how high a player can set their XP rate. While the Default XP Rate variable dictates what XP rate players start with and the rate will be set to if the user does `.xprate default`. As a recommendation, it would be a good idea to set the default and maximum experience rates to match. In this way, all players would initially have the maximum experience on the server, and then, through the `.xprate set` command, they could modify it.

# Player Commands

| Command        | Description                                       |
|----------------|---------------------------------------------------|
| `.xprate`      | Main Module Command                               |
| `.xprate view` | Displays the current XP rate                      |
| `.xprate set #`| Changes the XP rate to the value specified        |
| `.xprate default`| Returns the XP rate to the default value        |

> [!NOTE]
> The command is `.xprate` instead of `.xp` because Tortoise-WoW already has a core `.xp` command.

> [!IMPORTANT]
> If you no longer use the module, remember to delete any tables or values added within them.

### Database World

```sql
DELETE FROM `module_string` WHERE `module` = 'tw-mod-individual-xp';
DELETE FROM `module_string_locale` WHERE `module` = 'tw-mod-individual-xp';
```

### Database Characters

```sql
DROP TABLE `individualxp`;
```
