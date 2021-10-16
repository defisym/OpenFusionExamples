# Func

## Description

Call Functions via Expression or Action

## Properties

This object has no properties

## Action

- Call Func
  - *multiple or void return value*
- Call Func (Ternary)
  - *Call function A or B by the bool value*

- Return Value
  - *Note : ret values will be erased on next function call*
  - *Note : currently has a bug, if you call a function via expression, return value must be the last action of the event*

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
