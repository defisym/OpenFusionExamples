# EpicOnlineServices

## Description

EpicOnlineServices

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

- Unlock Achievement

## Condition

- On Login

## Expression

- Login Success
  - *0 = failed, 1 = success, used in `On Login` condition*
