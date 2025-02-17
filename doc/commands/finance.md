# Finance

## Amort

Amortizes a loan or investment based upon the current amortization settings.
Values must be stored in the TVM variables (`I%Yr`, `PV`, `Pmt`, and `PYr`). The
number of payments `n` is taken from the input together with flag –14
(`TVMPayAtBeginningOfPeriod` / `TVMPayAtEndOfPeriod`).

Given the number of payments `n`, the command deposits the value of the
principal, interest and balance in stack levels 1, 2 and 3.


## FinanceSolverMenu

Display the Time Value of Money (TVM) solver menu.

This menu can be used to directly resolve payment and interests problems.

## TVMPayAtBeginningOfPeriod

This flag indicates that payments occur at the beginning of a payment period.
For compatibility with HP calculators, flag `-14` can also be set.


## TVMPayAtEndOfPeriod

This flag indicates that payments occur at the end of a payment period.
For compatibility with HP calculators, flag `-14` can also be cleared.

## TVMRoot

Solves for the specified TVM variable using values from the remaining TVM
variables.
