# Project 3 Design Log
**Author:** <Xander Hong> · **P2 code used:** <my own>
**Tools used:** <Claude Code was used to create a breakdown of the spec into daily tasks with expected completion times. Claude was given the role of a C++ TA who helped explain smart pointer concepts, debug buggy sections of code, confirm proper functionality of methods and tests, and to help explain sections of the spec. No code was simply copied and pasted from Claude. All code was typed by-hand, but often revised after Claudes feedback.>

<!-- 500–800 words. An argument, not a diary: say what you decided, what
     you rejected, and what evidence backs it up. Delete these comments
     before submitting. -->

## 1. Hash table
For this AI harness, a hash table utilizing Chaining was implemented. Chaining was selected over Linear Probing because it is much simpler to implement the for_each(), add(), and find() functions using Chaining. Additionally, Chaining only produces around 6 worst-case chains while Linear Probing can cause many more (closer to 30). The Hash function FNV-1a was utilized for its robustness since it uses every key character, it's order sensitive, and it reliably returns the full-width value reduced to an index separately. 

## 2. Pointer stability
<!-- Why does a Tool* returned by find() survive a resize in your design? -->

## 3. Calculator trace
<!-- Trace the values and ops stacks for 2 * (3 + 4) - 5, one token at a
     time, in your own words. -->

## 4. Errors as values
<!-- Why do tools, resolve() and calc_evaluate() return errors instead of
     throwing? When IS an exception the right choice? -->

## 5. What I would design differently
