# Project 3 Design Log
**Author:** <Xander Hong> · **P2 code used:** <my own>
**Tools used:** <Claude Code was used to create a breakdown of the spec into daily tasks with expected completion times. Claude was given the role of a C++ TA who helped explain smart pointer concepts, debug buggy sections of code, confirm proper functionality of methods and tests, and to help explain sections of the spec. No code was simply copied and pasted from Claude. All code was typed by-hand, but often revised after Claudes feedback.>

<!-- 500–800 words. An argument, not a diary: say what you decided, what
     you rejected, and what evidence backs it up. Delete these comments
     before submitting. -->

## 1. Hash table
For this AI harness, a hash table utilizing Chaining was implemented. Chaining was selected over Linear Probing because it is much simpler to implement the for_each(), add(), and find() functions using Chaining. Additionally, Chaining only produces around 6 worst-case chains while Linear Probing can cause many more (closer to 30). The Hash function FNV-1a was utilized for its robustness since it uses every key character, it's order sensitive, and it reliably returns the full-width value, meaning it is not reduced by a modulo before being turned into an index. A helper-function called index_for() was created in order to macro the indexing of the key. This macro utilized a bitwise AND operation upon the hash(key) & (bucket_count - 1). This is valid because the bucket count always starts at a value of 8 buckets and only ever doubles, keeping the bucket_count always at a power of 2. The chosen threshold was a load factor of 1.0, which was reasonable for chaining where the average chain length equals alpha. Before add() completes correctly, a check occurs analyzing whether an add would put the load factor above the load threshold. If the load factor would exceed the load threshold, the hash table is resized, doubling the bucket_count and then re-inserting every tool back into the table using the new_bucket_count. This works each time since a key's index depends on the table size.

## 2. Pointer stability
<!-- Why does a Tool* returned by find() survive a resize in your design? -->

## 3. Calculator trace
<!-- Trace the values and ops stacks for 2 * (3 + 4) - 5, one token at a
     time, in your own words. -->

## 4. Errors as values
<!-- Why do tools, resolve() and calc_evaluate() return errors instead of
     throwing? When IS an exception the right choice? -->

## 5. What I would design differently
