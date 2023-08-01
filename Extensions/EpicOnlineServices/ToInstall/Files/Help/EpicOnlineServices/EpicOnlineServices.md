# EpicOnlineServices

## Description

EpicOnlineServices

## Note

Almost everything is **async**!

## Properties

- Initialize Options
  - *see: [Configuring and Creating the SDK](https://dev.epicgames.com/docs/en-US/game-services/eos-platform-interface#configuring-and-creating-the-sdk)*

- Platform Options
  - *see: [Creating the Platform Interface](https://dev.epicgames.com/docs/en-US/game-services/eos-platform-interface#creating-the-platform-interface)*

- Runtime Options
  - AuthType
    - *see: [Preferred Login Types for Epic Account](https://dev.epicgames.com/docs/en-US/epic-account-services/auth/auth-interface#preferred-login-types-for-epic-account)*
    - *To debug with DevAuthTool, disable VPN to avoid 502 error. Set portal: 6547, name: EOS.*
  - RequireLauncher
    - *see: [Restarting the App with the Launcher](https://dev.epicgames.com/docs/en-US/game-services/eos-platform-interface#restarting-the-app-with-the-launcher)*
  - RequireBootstrap
    - *some auth type requires to start through EOS boot strapper application, uncheck this to bypass it (**only should be used for debugging**)*

## Action

- Query
  - *Query new data from server, should be call periodically or on demand*

- Unlock Achievement

- Ingest Stat
  - *behavior depends on the type of stat*
  - *e.g., sum -> add value to the given stat*

## Condition

- On Login
- Login Success

- Query Complete
  - *Query achievement definition*
  - *Query stat*

## Expression

- Stat Value
  - *Get stat value, need query complete*
