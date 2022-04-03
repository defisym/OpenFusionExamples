# FindTheWay

## Description

A* Path Find, open source, 5X faster than Path Finding Object, all runtime compatible.

## Properties

This object has no properties

## Terminology

- Map type
  - Map is split to two part, terrain & dynamic, you don't need to compare if it's obstacle before handling dynamic part
  - the real map will be generated automatically once needed
  - Set : terrain & dynamic
  - Retrieve : real map & terrain & dynamic

- Cost
  - `0 ~ 255`, 255 = obstacle, others = path

- UseRealCoord
  - convert input to gird coord

- UseIterateCoord
  - use internal coord in `On ...` events

- Path name
  - keep it empty to retrieve last path

## Action

- Continue
  - *This extension put find/calc area in condition to check if params area valid. If you don't need following actions immediately, use this action to avoid fusion remove the empty event*
- Set Stash
  - *with this on, path & area will be stashed, then retrieve it with out calculate if states are the same (map/zoc/ignore flag, etc.)*
  - *stash will be clear automatically once map updated*
  - *disable stash will clear everything stashed*
  - *disable this may slightly improve performance as the state hash process is skipped*
  - *disable this if you encounter hash collision*

- Set Map
  - Set Map
  - Set Map By Object

  - Clear Map

- Path
  - Iterate Path

- Area
  - Create Object Zoc
    - iterate zoc position for object, trigger `On Create Object Zoc` for you to create real zoc area

  - Set Zoc By Object
  - Clear Zoc

  - Set Unit By Object
  - Clear Unit

  - Iterate Area

## Condition

- Map
  - Map Available
    - *extension maintains a internal class to handle everything. This class will be destructed then re-construct with new params in `Set Map` conditions. If params are invalid, those conditions will return false and all A/C/Es rely on this class will do nothing/return default value, use this to debug*
  - Check Map Cost At Coord

  - On Map Update
    - *once you update terrain or dynamic, extension won't update the map immediately as you may set multiple points*
    - *usually map is updated before find/calc area if needed, then this condition will be triggered*

- Set Map
  - Set Map By Size
  - Set Map By Base64
  - Set Map By Picture
  - Set Map By Collision
    - *you can let extension retrieve collision of layers, or call `On Set Map By Collision` for each gird for you to check & set collisions directly*

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
    - *check if zoc at calculated area*
  - Zoc At Object
    - *check if zoc and the given object has the same gird coord*

  - On Create Object Zoc

  - Calc Area
    - Line mode
      - no obstacle from start to the range (e.g., laser beam)
      - `allRange` & `allRangeAttackRange` are ignored in this mode
      - if `attack` is true, `ignore flag` will be taken into account
      - if `attack` is false, both map collision & all unit are taken into account, for skills that push/pull unit
    - Ring mode
      - if `allRange` is true, `range` will be used as move range, and `allRangeAttackRange` is used as maximum attack range, both move & attack area will be calculated in one short, e.g., danger zone of enemy.

  - On Iterate Area
  - On Iterate Attack Area
    - *if you `Calc Area` in all range mode, this condition will be true when iterate attack area in `On Iterate Area`*

  - Able to Iterate Area
    - *if you want create areas procedurally by `Iterate Area` with param once, this condition is here for you to check if it is able to iterate.*

- Object
  - Select All
    - *save fastloops when click on an area, move character to that point, then destroy all areas*

  - Object At Gird
  - Object At Coord
  - Object At Obstacle

  - Pick One Object At Object
    - *only display one instance when overlapping*

## Expression

- Iterate
  - *this values are shared by all `On ...` events, please do not nest them*

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
    - the same as `Path Found` but return steps directly, save events when calculation AI
  - Get Step of Path
  - Get Step Coord of Path

- Area
  - Get Ignore Flag
    - *`ignore` means this point doesn't treat as obstacle*
    - *A/C/E that requires this param need to update zoc/ally/enemy first to make ignore works properly*

  - Get Available Line Range
