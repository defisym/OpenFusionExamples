# EpicOnlineServices

## Description

EpicOnlineServices

## Note

Almost everything is **async**!
Wait operations complete before changing frame, or the game may crash!

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
    - *Permissions required when login, make sure you have enabled it in dev portal*
  - RequireLauncher
    - *see: [Restarting the App with the Launcher](https://dev.epicgames.com/docs/en-US/game-services/eos-platform-interface#restarting-the-app-with-the-launcher)*
  - RequireBootstrap
    - *some auth type requires to start through EOS boot strapper application, uncheck this to bypass it (**only should be used for debugging**)*

## Action

- Login
- Logout
  - *if `AuthType` is `Developer`, then you don't need to input accounts again when login*

- Query
  - *Query new data from server, should be call periodically or on demand*
  - *use the same type of `Query Complete` action*

- Unlock Achievement

- Ingest Stat
  - *behavior depends on the type of stat*
  - *e.g., sum -> add value to the given stat*

- Set Rich Text

## Condition

- On Error
  - *sometimes extension will ignore error and continue process, please read Epic documents*

- On Login
- Login Success

- Query Complete
  - *Query achievement definition -> `Achievement`*
  - *Query stat -> `Stat`*
  - *Query presence -> `Presence`*

## Expression

- Last Error

- Account ID
- Product User ID
  - *usually you don't need it, but useful for testing*

- Stat Value
  - *Get stat value, need query complete*

- Rich Text
  - *Get rich text, need query complete*
