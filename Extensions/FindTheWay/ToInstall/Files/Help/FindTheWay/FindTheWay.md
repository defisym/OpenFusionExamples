# FindTheWay

## Description

A* Path Find, open source, 5X faster than Path Finding Object, all runtime compatible.

## Note

Since build 295, fusion no longer supports to call immediate events from conditions and expressions, so please avoid using `On Map Update` and `Set Map By Collision`'s event iterate mode.

## Properties

- Settings
  - Isometric
    - *use isometric mode*
    - *in isometric mode, use `GIGS( >Isometric Grid Width<, >Isometric Grid Height< )` to generate valid gridSize, then pass it to A/C/Es that ask for gridSize*

## Terminology

- Map type
  - Map is split to two part, terrain & dynamic, you don't need to compare if it's obstacle before handling dynamic part
  - the real map will be generated automatically once needed
  - Set : terrain & dynamic
  - Retrieve : real map & terrain & dynamic

- Grid offset
  - if play area is smaller than frame size, and you need to make it at the center, then here comes the offset to main frame, this param is needed to convert coord properly.

- Cost
  - `0 ~ 255`, 255 = obstacle, others = path

- UseRealCoord
  - convert input to grid coord

- UseIterateCoord
  - use internal coord in `On ...` events

- Path name
  - keep it empty to retrieve last path

- Creation
  - all objects created by this extension will be put to layer 1 by default, please move then to target layer manually if needed

## Action

- Continue
  - *This extension put find/calc area in condition to check if params area valid. If you don't need following actions immediately, use this action to avoid fusion remove the empty event*

- Set Stash
  - *with this on, path & area will be stashed, then retrieve it with out calculate if states are the same (map/zoc/ignore flag, etc.)*
  - *stash will be clear automatically once map updated*
  - *disable stash will clear everything stashed*
  - *disable this may slightly improve performance as the state hash process is skipped*
  - *disable this if you encounter hash collision*

- Map
  - Iterate Map

- Set Map
  - Set Map
  - Set Map By Object

  - Clear Map

- Grid
  - Create Grid
    - *paste grid object as backdrop to non-obstacle grid, pasted backdrops will has the same effect (alpha blending coef for example) of grid object*
    - *pasted backdrop has no effect on collisions, aka `OBSTACLE_TRANSPARENT`*
    - *grid object **must** exist*
    - *grid object **must** has the **same size** of map grid, grid object's hot spot **must** be set to **center***
    - *the first frame of grid object's `stopped` animation sequence's direction `0` is used as top & left border, second is used as right border and third for bottom*
    - *this action will paste first frame to all walkable grid, and if the one right to current grid is obstacle, second frame is also pasted with coordinate offset, third frame is pasted if the one below is obstacle*
    - *usually you will notice a 'grid line width in pixel' offset when closing to obstacle, this action will use grid object's first & second alterable value to offset back*
    - *the first alterable value is right offset and the second is bottom offset, set it to the width of grid line (in pixels)*
  
  - Set Grid Size
    - *when using `Set Map By Size`, `Set Map By Base64` or `Set Map By Picture`, you need to set grid size to make coord convert work properly*

- Path
  - Iterate Path

- Area
  - Create Object Zoc
    - *iterate zoc position for object, auto create zoc area, coord param is ignored*
  - Create Object Zoc By Name
    - *same as above*
  - Create Object Zoc By Event
    - *iterate zoc position for object, trigger `On Create Object Zoc` for you to create real zoc area*

  - Set Zoc By Object
  - Clear Zoc

  - Set Unit By Object
  - Clear Unit

  - Iterate Area

  - Create Area Once
  - Create Area By Name Once

  - Create AOE
    - *Create different AOE area, check appendix for type declaration*

## Condition

- Map
  - Map Available
    - *extension maintains a internal class to handle everything. This class will be destructed then re-construct with new params in `Set Map` conditions. If params are invalid, those conditions will return false and all A/C/Es rely on this class will do nothing/return default value, use this to debug*
  - Check Map Cost At Coord

  - On Map Update
    - *once you update terrain or dynamic, extension won't update the map immediately as you may set multiple points*
    - *usually map is updated before find/calc area if needed, then this condition will be triggered*
  - On Iterate Map

- Set Map
  - Set Map By Size
    - *please update gird size later*
  - Set Map By Base64
    - *please update gird size later*

  - Set Map By Active
    - *in this mode, `gridSize` & `gridOffset` are used to find the proper pixel of picture, please update gird size later*
    - *picture should be in traditional mode, even map mode is set to isometric*
    - *red channel will be used as `terrain` cost and green channel is `dynamic` cost*
    - *uses the frame from an active object's `stopped` animation sequence's direction `0`*
    - *source object **must** exist*
  - Set Map By Picture
    - *ditto*
    - *ditto*
    - *ditto*
    - *load from file*
  - Set Map By Surface
    - *ditto*
    - *ditto*
    - *ditto*
    - *load from `cSurface*`, extension will cast input to pointer the try to retrieve*

  - Set Map By Collision
    - *you can let extension retrieve collision of layers, or call `On Set Map By Collision` for each grid for you to check & set collisions directly*

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
  - Select
    - Select All
      - *save fastloops when click on an area, move character to that point, then destroy all areas*

    - Object At Grid
    - Object At Coord
    - Object At Obstacle

    - Object At Object
      - *check if former object and the latter object has the same grid coord*

    - Pick One Object At Object
      - *only display one instance when overlapping itself*

  - Non-Select
    - No Object At Grid
    - No Object At Coord

## Expression

- Iterate
  - *this values are shared by all `On ...` events, please do not nest them*

  - Get Iterate X
  - Get Iterate Y
  - Get Iterate Index

- Grid
  - Get Isometric Grid Size
    - *used in isometric mode, to generate valid gridSize*
    - *algorithm:*

      ```C++
        isoGridWidth = isoGridWidth & 0xFFFF;
        isoGridHeight = isoGridHeight & 0xFFFF;

        return (isoGridHeight << 16) | isoGridWidth;
      ```

    - *you can pass this even if you are under traditional mode, in that case only low 16 bit are used as grid size*

  - Get Grid Coord
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

## Appendix

### AOE

if attack unit at blue grid, other valid units (usually the ally of the former one) at yellow grid will also take splash damage.

#### Type 11

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="313" style="width:235.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 12

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="313" style="width:235.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:5;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:6;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 21

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="313" style="width:235.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 22

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="313" style="width:235.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 23

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="439" style="width:329.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 24

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="439" style="width:329.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><b><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></b></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 25

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="439" style="width:329.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:5;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 26

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="439" style="width:329.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;mso-background-themecolor:background1;mso-background-themeshade:
  128;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:5;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 31

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="313" style="width:235.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 32

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="439" style="width:329.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:5;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:6;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>

#### Type 41

<div align="center">

<table class="MsoNormalTable" border="0" cellspacing="0" cellpadding="0" width="313" style="width:235.0pt;border-collapse:collapse;mso-yfti-tbllook:1184;
 mso-padding-alt:0cm 5.4pt 0cm 5.4pt">
 <tbody><tr style="mso-yfti-irow:0;mso-yfti-firstrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  mso-border-alt:solid windowtext .5pt;background:gray;padding:0cm 5.4pt 0cm 5.4pt;
  height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-left:none;mso-border-top-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:1;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:2;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:#00B0F0;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:3;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:yellow;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
 <tr style="mso-yfti-irow:4;mso-yfti-lastrow:yes;height:40.05pt">
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border:solid windowtext 1.0pt;
  border-top:none;mso-border-left-alt:solid windowtext .5pt;mso-border-bottom-alt:
  solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;background:
  gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
  <td width="63" nowrap="" valign="bottom" style="width:47.0pt;border-top:none;
  border-left:none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  mso-border-bottom-alt:solid windowtext .5pt;mso-border-right-alt:solid windowtext .5pt;
  background:gray;padding:0cm 5.4pt 0cm 5.4pt;height:40.05pt">
  <p class="MsoNormal" align="left" style="text-align:left;mso-pagination:widow-orphan"><span style="font-size:11.0pt;font-family:等线;mso-bidi-font-family:宋体;color:black;
  mso-font-kerning:0pt">　<span lang="EN-US"><o:p></o:p></span></span></p>
  </td>
 </tr>
</tbody></table>

</div>
