# NeoPic

## Description

Enable the power to load encrypted image files, to store them to a lib that keeps data over frames, or to do something like stack blur or affine transformation.

## Properties

- Lib Settings
  - Lib Object
    - *Lib Object will hold surfaces for other objects, you can retrieve them by filename. Lib will keep them over frames.*

## Action

- Load
  - Load From File
    - *keep key empty if file is not encrypted*
  - Load From Lib
    - *keep key empty if file is not encrypted*
    - *if target lib doesn't contain target file, lib will try loading that file first, if success, it will be added to lib*

## Condition

## Expression
