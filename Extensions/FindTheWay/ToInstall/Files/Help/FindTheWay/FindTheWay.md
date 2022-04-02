# FindTheWay

## Description

A* Path Find, open source, 5X faster than Path Finding Object, all runtime compatible.

## Properties

This object has no properties

## Action

- Continue
- Set Stash

- Set Map
  - Set Map
  - Set Map By Object

  - Clear Map

- Path
  - Iterate Path

- Area
  - Create Object Zoc

  - Set Zoc By Object
  - Clear Zoc

  - Set Unit By Object
  - Clear Unit

  - Iterate Area

## Condition

- Map
  - Map Available
  - Check Map Cost At Coord

  - On Map Update

- Set Map
  - Set Map By Size
  - Set Map By Base64
  - Set Map By Picture
  - Set Map By Collision

  - On Set Map By Collision

- Path
  - Path Found
  - Path Available

  - On Iterate Path

- Area
  - Zoc Valid
    - *not at obstacle or unit*
    - *need to update unit before using*
  - Zoc At Area
  - Zoc At Object

  - On Create Object Zoc

  - Calc Area

  - On Iterate Area
  - On Iterate Attack Area

  - Able to Iterate Area

- Object
  - Select All

  - Object At Gird
  - Object At Coord
  - Object At Obstacle

  - Pick One Object At Object

## Expression

- Iterate
  - Get Iterate X
  - Get Iterate Y
  - Get Iterate Index

- Gird
  - Get Gird Coord
  - Get Real Coord

- Map
  - Get Map Cost
  - Get Map Base64
  - Get Map String

  - Get MapType Map
  - Get MapType Terrain
  - Get MapType Dynamic

  - Get Map Cost Path
  - Get Map Cost Obstacle
  - Get Map Cost Valid

- Path
  - Get Step
  - Get Step of Path
  - Get Step Coord of Path

- Area
  - Get Ignore Flag
    - *`ignore` means this point doesn't treat as obstacle*
    - *A/C/E that requires this param need to update zoc/ally/enemy first to make ignore works properly*

  - Get Available Line Range
