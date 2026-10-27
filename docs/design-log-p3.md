# Project 3 Design Log
**Author:** <Xander Hong> · **P2 code used:** <my own>
**Tools used:** <Claude Code was used to create a breakdown of the spec into daily tasks with expected completion times. Claude was given the role of a C++ TA who helped explain smart pointer concepts, debug buggy sections of code, confirm proper functionality of methods and tests, and to help explain sections of the spec. No code was simply copied and pasted from Claude. All code was typed by-hand, but often revised after Claudes feedback.>

<!-- 500–800 words. An argument, not a diary: say what you decided, what
     you rejected, and what evidence backs it up. Delete these comments
     before submitting. -->

## 1. Hash table
For this AI harness, a hash table utilizing Separate Chaining was implemented. Separate Chaining was selected over Linear Probing because it is much simpler to implement the for_each(), add(), and find() functions using Separate Chaining. Additionally, Separate Chaining only produces a worst-case chain of about 6 while Linear Probing causes a much greater worst-case chain (~30). The Hash function FNV-1a was utilized for its robustness since it uses every key character, it's order sensitive, and it reliably returns the full-width value, meaning it is not reduced by a modulo before being turned into an index. A helper-function called index_for() was created in order to macro the indexing of the key. This macro utilized a bitwise AND operation upon the hash(key) & (bucket_count - 1). This is valid because the bucket count always starts at a value of 8 buckets and only ever doubles, keeping the bucket_count always at a power of 2. The chosen threshold was a load factor of 1.0, which was reasonable for separate chaining where the average chain length equals alpha. Before add() completes correctly, a check occurs analyzing whether an add would put the load factor above the load threshold. If the load factor would exceed the load threshold, the hash table is resized, doubling the bucket_count and then re-inserting every tool back into the table using the new_bucket_count. This works each time since a key's index depends on the table size. During the reg_scale_10000 test, the FNV-1a hash function was compared with a weaker "s[0] + s.size()" hash function. The data was as follows:
                    Buckets  |  Load Factor  | Max-Chain
FNV-1a:              16384         ~0.61          6
"s[0] + s.size()":   16384         -----          9000

The weaker hash in this case fails because every name starts with t and the only difference is the length. This only returns 4 distinct hash values and totally degrades the find() function's runtime from O(1) to O(n). Because the hash is so weak, producing more buckets is unhelpful since identical hashes collide irregardless of bucket_count. With chaining, the system's load factor returned to be less than 1 with a value of ~0.61. This means that most typically, a random bucket holds either a value of 0 or 1 tool and a typical find() function compares about 1 name. That's why it is expected that find()'s runtime is O(1).

## 2. Pointer stability
In this design a Tool* returned by find() survives a resize because the buckets that get resized are holding an object called std::unique_ptr<Tool> that has an owning pointer that points to the object but is not the object itself. This is useful because it means that when changing buckets, rehashing can move the owning addresses between buckets but the objects themselves never move, meaning the pointers to them stay valid.

## 3. Calculator trace
First, the `2` is pushed onto the top of the values stack | `2` |
Second, the `*` is pushed onto the top of the ops stack | `*` |
Third, the `(` is pushed onto the top of the ops stack | `* (` |
Fourth, the `3` is pushed onto the top of the values stack | `2 3` |
Fifth, the `+` is pushed onto the top of the ops stack | `* ( +` |
Sixth, the `4` is pushed onto the top of the values stack | `2 3 4` |
Seventh, because a `)` has appeared, the operation(s) within the parentheses are force to appy. Apply '+' (3+4=7); top is now `(` → pop it; The value stack is | `2 7` | and the ops stack is | `*` |
Eighth, because an operator has appear, a check for precedence occurs. Because `*` has greater precedence than `-`, and because it is previously on the stack top, `*` is applied (2*7=14). The values stack is | `14` |. Afterwards `-` is pushed onto the top of the ops stack | `-` |
Nineth, the `5` is pushed onto the top of the values stack | `14 5` |
Tenth, because there are no more tokens being processed, the final operator is applied `-` (14-5=9). The ops stack is |``| and the values stack is |`9`|.
Tenth, because there are no more operators left in the ops stack, the last values value `9` is placed into *out and return is true.

## 4. Errors as values
<!-- Why do tools, resolve() and calc_evaluate() return errors instead of
     throwing? When IS an exception the right choice? -->

## 5. What I would design differently
