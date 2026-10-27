# Project 3 Design Log
**Author:** <Xander Hong> · **P2 code used:** <my own>
**Tools used:** <Claude was used to create a breakdown of the spec into daily tasks with expected completion times. Claude was given the role of a C++ TA who helped explain smart pointer concepts, debug buggy sections of code, confirm proper functionality of methods and tests, and to help explain sections of the spec. No code was simply copied and pasted from Claude. All code was typed by-hand, but often revised after Claude`s feedback.>

## 1. Hash table
For this AI harness, a hash table utilizing Separate Chaining was implemented. Separate Chaining was selected over Linear Probing because a chaining table can’t fill up, so find() can never loop forever, and collisions stay local instead of forming clusters. The Hash function FNV-1a was utilized for its robustness since it uses every key character, it's order sensitive, and it reliably returns the full-width value, meaning it is not reduced by a modulo before being turned into an index. A helper-function called index_for() was created to index the key. This helper-function utilized a bitwise AND operation upon the hash(key) & (bucket_count - 1). This is valid because the bucket count always starts at a value of 8 buckets and only ever doubles, keeping the bucket_count always at a power of 2. The chosen threshold was a load factor of 1.0, which was reasonable for separate chaining where the average chain length equals alpha. Before inserting, add() checks whether the new entry would push the load factor past 1.0; if so, it doubles the bucket count and re-inserts every tool. This works each time since a key's index depends on the table size. During the reg_weak_hash_comparison test, the FNV-1a hash function was compared with a weaker "s[0] + s.size()" hash function. The data was as follows:
                    Buckets  |  Load Factor  | Max-Chain
FNV-1a:              16384         ~0.61          6
"s[0] + s.size()":   16384         -----          9000

The weaker hash in this case fails because every name starts with t and the only difference is the length. This only returns 4 distinct hash values and totally degrades the find() function's runtime from O(1) to O(n). Because the hash is so weak, producing more buckets is unhelpful since identical hashes collide regardless of bucket_count. A resize from n to 2n buckets costs O(n), but it only happens after about n cheap inserts since the last one, so each insert pays O(1) extra on average.

## 2. Pointer stability
In this design a Tool* returned by find() survives a resize because the buckets that get resized are holding an object called std::unique_ptr<Tool> that has an owning pointer that points to the object but is not the object itself. This is useful because it means that when changing buckets, rehashing can move the owning addresses between buckets but the objects themselves never move, meaning the pointers to them stay valid.

## 3. Calculator trace
First, `2` is pushed onto the values stack | `2` |
Second, `*` is pushed onto the ops stack | `*` |
Third, `(` is pushed onto the ops stack | `* (` |
Fourth, `3` is pushed onto the values stack | `2 3` |
Fifth, `+` is pushed onto the ops stack | `* ( +` |
Sixth, `4` is pushed onto the values stack | `2 3 4` |
Seventh, because a `)` has appeared, the operation(s) within the parentheses are forced to apply. Apply '+' (3+4=7); top is now `(` so pop it; The value stack is | `2 7` | and the ops stack is | `*` |
Eighth, because an operator has appeared, a check for precedence occurs. Since `*` has greater precedence than `-` because its precedence value is greater than or equal to it (`*` precedence of 2 >= `-` precedence of 1) and it was previously on the stack top, `*` is applied (2*7=14). The values stack is | `14` |. Afterwards `-` is pushed onto the ops stack | `-` |
Ninth, the `5` is pushed onto the values stack | `14 5` |
Tenth, because there are no more tokens being processed, the final operator is applied `-` (14-5=9). The ops stack is |``| and the values stack is |`9`|. Leaving 9 as the result.

## 4. Errors as values
For models, tool failures are normal and expected since all tools typically depend on model input. For example, a missing file, a forbidden path, or a division by zero are all common examples of a tool functioning properly by giving an error. When this happens, instead of shutting down, the model needs to read the error and try something else; in other words, failures are returned as data. Exceptions to this rule include programmer bugs (i.e. like calling as_string() on a number). This is why falses return with clear reasons that tie back to my code, like "path escapes the workspace" or "malformed expression": the clearer the reason for the failure, the more reliably the model will be able read and correct the error when trying something new.

## 5. What I would design differently

Initially, when I was designing the calc_evaluate() function, I determined that a helper-function named `bool apply()` would be a helpful tool--especially in computing parenthesized expressions; however, my first iteration of the apply() function attempted to evaluate whole parenthesized groups by constantly rescanning precedence on several operators within a big expression to compare them and find the greatest precedence. Through troubleshooting that design, I learned that the two stack algorithm is best implemented by keeping apply() to a single operation and putting all the ordering logic in the main loop.