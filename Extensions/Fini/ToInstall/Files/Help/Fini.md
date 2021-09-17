# Fini

## Description

Fini, based on brofield's brilliant work simpleini, is a faster ini ext for fusion.

![BenchMark](BenchMark.png)

about 200X faster when writing and 10X faster when reading, but you need to **load/save file manually**.

## Properties

This object has no properties

## Action

- New
  - *you need this action to initialize.*
- Release
  - *release the data in memory.*
- Load From File
  - *if you load from file, extension will auto initialize itself.*
  - *if the file is not encrypted, keep the key section empty.*
- Save to File
  - *if you don't want to encrypt the file, keep the key section empty.*
- Load From String
  - *load from string, e.g.`[Sec]/r/nItem=Value`*
- Set Value (Section - Item)
  - *Note: you can only set values in a section, doesn't support item-only file like MMF.*
- Set String (Section - Item)
  - *Note: you can only set values in a section, doesn't support item-only file like MMF.*
- Delete (Section - Item)
  - *Keep Item empty to delete the entire section.*
  - *Note: if a scetion is empty after you delete a item, this section will be deleted automatically*
- Iterate
  - Iterate Sections
  - Iterate Items
    - *need section name to start iterate.*
    - *if you start loop, "Currenct Section" will be set to the section name you inputted*

## Condition

- Iterate
  - On Iterate Sections
  - On Iterate Items

## Expression

- Get Value (Section - Item)
  - *support float, e.g. -15.2, +13.7, 114.514, if the value is not a number, return 0*
  - *return 0 if there is no item match.*
  - *Note: you can only read values in a section, doesn't support item-only file like MMF.*
- Get String (Section - Item)
  - *return "" if there is no item match.*
  - *Note: you can only read values in a section, doesn't support item-only file like MMF.*
- Get Current Section
  - *get the section name while iterating.*
- Get Current Item
  - *get the item name while iterating.*
- Save to String
  - *save the entire file to string.*