# Func

## Description

Call Functions via Expression or Action

## Properties

This object has no properties

## Known issue

First function call will cost much more time than the second and following ones

## Note

To use this object, you need to update to `R293.9`, which contains a fix to avoid crash when calling event via expression [Changelog](http://www.clickteam.com/webftp/files/mmf2/ChangeLogs/293.9.txt)

Or you need to use the code below to replace `CallEvent(ONFUNC)` in function `CallFuncCore` at `Func.h`, then build your own `.mfx`(don't forget to change the Inc/Lib/Output path in properties)

```C++
// Note: if your MMF version is below R293.9, you need to use code below instead of just a single line CallEvent(ONFUNC) to avoid crash

LPRH pRh = rdPtr->rHo.hoAdRunHeader;
expression* saveExpToken = pRh->rh4.rh4ExpToken;
CallEvent(ONFUNC);
pRh->rh4.rh4ExpToken = saveExpToken;

//Call Func;
//CallEvent(ONFUNC);
```

## Action

- Call Func
  - *multiple or void return value*
- Call Func (Ternary)
  - *Call function A or B by the bool value*

- Loop
  - *Please use this inside a func event to avoid unexpected behaviour*
  - *Loop control for action funcs, works alomst the same as fastloop, but make it easier to pass temp values*
  
  - Set Func's LoopIndex
  - Stop Func Loop

- Temp Param
  - *Please use this inside a func event to avoid unexpected behaviour*
  - *You can set several temp params, and retrieve then for calculation, etc*
  - *Temp params are saved in a map, so recursive funcs share the same temp param. That is to say, temp params of a func is not separated as `C/C++` if you call it recurisively, use normal param to pass temp values in this case*
  - *all temp params of current func will be erased after func return (`On Func` event finish) if it's recursive index is 1*

  - Set Func's Temp Param
    - *use this to change or set parent func's temp param in it's child*
    - *Althougth you can set a never called func's temp param, but actually I do not recommend this*
  
  - Set Current Func's Temp Param
    - *like the action above but only effect the current func(recommended)*

- Return Value
  - *Please use this inside a func event to avoid unexpected behaviour*
  - *Note : ret values will be erased on next function call*
  
  - Set Current Func's Return
    - *Clear return vector and set return use the same syntax of `Call Func` param*

  - Set Current Func's Return Value
    - *Clear return vector and set a return value*

  - Push Current Func's Return Value
    - *Add a return value to current function*
    - *if function is called by expression, expression will only return the first return value you pushed by this action*
    - *use action `Call Func` above to return multiple values*

## Condition

- On Func
  - *On function call*

- Validity
  - *Please use this inside a func event to avoid unexpected behaviour*
  
  - Return
    - Previous Func Has Return At
      - *check if last func has return value at target pos*

  - Param
    - Curernt Func Has Param At
      - *check if curernt func has param at target pos*

  - Temp Param
    - Func Has Temp Param
      - *check if target func has temp param of target name*
    - Curernt Func Has Temp Param
      - *check if current func has temp param of target name*

## Expression

- Call Func
  - *Use function name and params to call and loop a function*
  - *if you encountered crash issues, and your fusion is older than `R293.9`, please check the `Note` section and follow steps to build a compatible version*
  
  - Call Func (return value)
  - Call Func (return string)

- Ternary
  - *return bool?A:B*
  
  - Ternary (return value)
  - Ternary (return string)

- Get Param
  - *Please use this inside a func event to avoid unexpected behaviour*
  - *Get param of current function by position*
  - *Position starts by 0*
  
  - Get Current Func's Param (return value)
  - Get Current Func's Param (return string)

  - Get Current Func's Number of Params

- Get Temp Param
  - *Please use this inside a func event to avoid unexpected behaviour*
  - *Get temp param by funcname & paramname*
  - *return `0`(value) or empty string `""` (string) if no match*
  
  - Get Func's Temp Param (return value)
    - *return `0` if target pos is not a value*
  - Get Func's Temp Param (return string)

  - *Get current func's temp param by paramname*
  - *return `0`(value) or empty string `""` (string) if no match*
  
  - Get Current Func's Temp Param (return value)
    - *return `0` if target pos is not a value*
  - Get Current Func's Temp Param (return string)

- Get Return
  - *Please use this inside a func event to avoid unexpected behaviour*
  - *Note : ret values will be erased on next function call*
  - *Get return value of current function by position*
  - *Position starts by 0*
  - *Expressions must return a value, so if you forget to set it, app will crash*
  - *return `0`(value) or empty string `""` (string) if there is no value at the pos you want to retrieve*
  
  - Get Previous Func's Return Value (return value)
    - *return `0` if target pos is not a value*
  - Get Previous Func's Return Value (return string)

  - Get Previous Func's Number of Return Values

- Get Index
  - Get Func's LoopIndex
    - *Please use this inside a func event to avoid unexpected behaviour*
    - *return -1 if no func match*

  - Get Func's Recursive Index
    - *Please use this inside a func event to avoid unexpected behaviour*
    - *Get current call num of current function in function stack*
    - *check `Recursive` section in example mfa to get more information about usage*
    - *return -1 if no func match*

- Get Current FuncName
  - *Please use this inside a func event to avoid unexpected behaviour*
  - *e.g. when retriving temp values of current func, `GetTempParam$( "Func", CurFuncName$( "Func" ), "ParamName" )`*
