# Func

## Description

Call Functions via Expression or Action

## Properties

This object has no properties

## Note

To use this object, you need to update to `R293.9`, which contains a fix to avoid crash when calling event via expression [Changelog](http://www.clickteam.com/webftp/files/mmf2/ChangeLogs/293.9.txt)

Or you need to use the code below to replace `CallEvent(ONFUNC)` in function `CallFuncCore` at `Func.h`, then build your own `.mfx`(don't forget to change the Inc/Lib/Output path in properties)

```C++
// Note: if your MMF version is under R293.9, you need to use code below instead of just a single line CallEvent(ONFUNC) to avoid crash

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

- Temp Param
  - *You can set several temp params, and retrive then for calculation, etc*
  - *all temp params of current func will be erased after process*

  - Set Temp Param
    - *use this to change or set parent func's temp param in it's child*
    - *Althougth you can set a never called func's temp param, but actually I do not recommend this*
  - Set Temp Param of Current Func
    - *like the action above but only effect the current func(recommended)*

- Return Value
  - *Note : ret values will be erased on next function call*
  - *Note : currently has a bug, if you call a function via expression, return value must be the last action of the event*

  - Set Return
    - *Clear return vector and set return use the same syntax of `Call Func` param*

  - Set Return Value
    - *Clear return vector and set a return value*

  - Push Return Value
    - *Add a return value to current function*
    - *if function is called by expression, expression will only return the first return value you pushed by this action*
    - *use action `Call Func` above to return multiple values*

## Condition

- On Func
  - *On function call*

## Expression

- Call Func
  - *Use function name and params to call a function*
  
  - Call Func (return value)
  - Call Func (return string)

- Ternary
  - *return bool?A:B*
  
  - Ternary (return value)
  - Ternary (return string)

- Get Param
  - *Get param of current function*
  - *Position starts by 0*
  
  - Get Param (return value)
  - Get Param (return string)

  - Get Param Size

- Get Temp Param
  - *Get temp param by funcname & paramname*  
  - Get Temp Param (return value)
  - Get Temp Param (return string)

  - *Get current func's temp param by paramname*
  - Get Current Temp Param (return value)
  - Get Current Temp Param (return string)

- Get Return
  - *Note : ret values will be erased on next function call*
  - *Get return value of current function*
  - *Position starts by 0*
  
  - Get Return (return value)
  - Get Return (return string)

  - Get Return Size

- Get Recursive Index
  - *Get current call num of current function in function stack*
  - *check `Recursive` section in example mfa to get more information about usage*

- Get Current FuncName
  - *e.g. when retriving temp values of current func, `GetTempParam$( "Func", CurFuncName$( "Func" ), "ParamName" )`*
