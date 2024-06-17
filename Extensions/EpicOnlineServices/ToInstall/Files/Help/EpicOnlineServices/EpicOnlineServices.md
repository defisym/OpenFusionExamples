# EpicOnlineServices

## Description

EpicOnlineServices

## Note

Almost everything is **async**!
Extension will wait until all callbacks finishes. This may cause lags.

If your game supports other platform, e.g., Steam, you can create a file along with the execution file to disable everything. You should grantee no platform features will be called, or may crash. The easiest way is use `Platform Enabled` condition to disable event group.

for example, exe path is `C:\YourGame\Game.exe`, then you can create file `C:\YourGame\Platform\NoEpic` to disable `EpicOnlineServices`. This filename is hard coded.

## Properties

- Initialize Options
  - *see: [Configuring and Creating the SDK](https://dev.epicgames.com/docs/en-US/game-services/eos-platform-interface#configuring-and-creating-the-sdk)*

- Platform Options
  - *see: [Creating the Platform Interface](https://dev.epicgames.com/docs/en-US/game-services/eos-platform-interface#creating-the-platform-interface)*

- Runtime Options
  - AuthType
    - *see: [Preferred Login Types for Epic Account](https://dev.epicgames.com/docs/en-US/epic-account-services/auth/auth-interface#preferred-login-types-for-epic-account)*
    - *To debug with DevAuthTool, disable VPN to avoid 502 error. Set portal: 6547, name: EOS.*
  - Auth Permissions
    - *Permissions required when login, make sure you have enabled it in dev portal, edit in `Your Product` -> `Epic Account Services` -> `APPLICATIONS` -> `Permissions`*
    - *if permission mismatch (both more than or less than), you'll get a infinite refresh in login overlay with internal error code `18206`, aka `Corrective action is required to continue`*
    - *see also: [this thread](https://eoshelp.epicgames.com/s/question/0D54z00009EYZ8bCAH/in-unreal-engine-5-eosauthlogin-never-recieve-completed-callback?language=en_US)*
  - AutoLogin
    - *auto login when first object is created*
  - AutoLogout
    - *auto logout when you exit & restart app*
    - *app will freeze to wait for the logout callback*
  - RequireLauncher
    - *see: [Restarting the App with the Launcher](https://dev.epicgames.com/docs/en-US/game-services/eos-platform-interface#restarting-the-app-with-the-launcher)*
  - RequireBootstrap
    - *some auth type requires to start through EOS boot strapper application, uncheck this to bypass it (**only should be used for debugging**)*

## Action

- General
  - Query
    - *Query new data from server, should be call periodically or on demand*
    - *use the same type of `Query Complete` action*

- Auth
  - Login
  - Logout
    - *if `AuthType` is `Developer`, then you don't need to input accounts again when login*

- Achievement && Stat
  - Unlock Achievement

  - Ingest Stat
    - *behavior depends on the type of stat*
    - *e.g., sum -> add value to the given stat*

- Presence
  - Set Rich Text

## Condition

- Platform Enabled

- General
  - On Error
    - *sometimes extension will ignore error and continue process, please read Epic documents*

  - Query Complete
    - *Query achievement definition -> `Achievement`*
    - *Query stat -> `Stat`*
    - *Query presence -> `Presence`*

- Auth
  - On Login
  - On Logout

  - User Login

## Expression

- General
  - Last Error

- Auth
  - Account ID
  - Product User ID
    - *usually you don't need it, but useful for testing, e.g., reset user achievement in dev portal*

- Achievement && Stat
  - Stat Value
    - *Get stat value, need query complete*

- Presence
  - Rich Text
    - *Get rich text, need query complete*
