# c_hash_test

Toy implementation of a hashtable in C. Implements linked lists, a struct for holding pairs (key/values) and the hashtable itself with a toy hashing algorithm. Does not yet implement deletion or resizing/rehashing the hashtable.

I wrote this as an exercise to see if i even remembered how to write C code. Also i was thinking about how having a programming environment with a get/set/dict type data structure brings it up to a much higher level of convenience in terms of what algorithms can be quickly implemented. It also occurs to me that with a little more work, if you implement your own `malloc()`, `free()` and a few `string.h` functions and you cross compile to an embedded target then you don't even need an operating system to run this. So in a way this is the C programming equivalent of "can you start a fire in the woods without matches?".

## Compile and Run

```
$ gcc hash.c -o hash ; strip hash
$ ./hash
```
