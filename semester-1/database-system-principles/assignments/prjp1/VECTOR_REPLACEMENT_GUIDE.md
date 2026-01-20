# Guide: Removing C++ Vectors from db.cpp

## Summary

Your `db.cpp` currently uses C++ standard library features (`std::vector`, `std::string`, `std::algorithm`) which require compiling with `-lstdc++`. These are only used in the `sem_select` function for SELECT query processing.

## Why Vectors Were Used

Vectors were convenient for:
1. Dynamic arrays of SELECT conditions (WHERE clauses)
2. Column names and aggregate functions
3. Storing variable-length result rows
4. Join column pairs
5. Table data buffers

## Solution: Replace with C-style Code

### Changes Already Made

1. ✅ Removed C++ headers from `db.cpp`:
   - Removed `#include <vector>`
   - Removed `#include <algorithm>`  
   - Removed `#include <string>`
   - Removed `#include <iostream>`

2. ✅ Removed `#include <vector>` from `db.h`

3. ✅ Updated `ResultRow` struct in `db.h`:
   ```c
   struct ResultRow {
       unsigned char* l_rec;      // left table record
       int l_rec_size;
       unsigned char* r_rec;      // right table record (for joins)
       int r_rec_size;
   };
   ```

4. ✅ Updated `get_value()` helper function signature:
   ```c
   bool get_value(const unsigned char* rec, int rec_size, int offset, int type, 
                  int* val_int, char* val_str, bool* is_null);
   ```

### Changes Still Needed

The main work is replacing vectors in `sem_select()` function. I've created a complete C-only version in `sem_select_c_version.c` that you can use to replace the existing function.

### Key Replacements

| C++ Code | C Replacement |
|----------|---------------|
| `std::vector<SelectCondition> conditions` | `SelectCondition conditions[MAX_NUM_COL]; int num_conditions = 0;` |
| `std::vector<std::string> select_cols` | `char select_cols[MAX_NUM_COL][MAX_TOK_LEN]; int num_select_cols = 0;` |
| `std::vector<int> agg_funcs` | `int agg_funcs[MAX_NUM_COL]; int num_agg_funcs = 0;` |
| `std::vector<ResultRow> results` | `ResultRow* results = malloc(...); int num_results = 0;` |
| `std::vector<unsigned char> data` | `unsigned char* data = malloc(...);` |
| `conditions.push_back(cond)` | `conditions[num_conditions++] = cond;` |
| `results.size()` | `num_results` |
| `std::sort(results.begin(), results.end(), ...)` | Bubble sort with C arrays |

### How to Apply the Changes

Option 1: **Manual replacement** (recommended for understanding)
- Open `db.cpp` and `sem_select_c_version.c` side-by-side
- Replace the entire `sem_select` function (lines ~1999-2558) with the version from `sem_select_c_version.c`

Option 2: **Let me do it** (if you want me to complete the replacement)
- I can finish replacing the function automatically

## After Replacement

### Compile Command Changes

**Before:**
```bash
gcc -g -o db db.cpp -lstdc++
```

**After:**
```bash
gcc -g -o db db.cpp
# OR
gcc -g -o db db.c  # Can even rename to .c
```

### Update test_checklist.sh

Change line 48 from:
```bash
gcc -g -o db db.cpp -lstdc++
```

To:
```bash
gcc -g -o db db.cpp
```

## Benefits

1. ✅ No C++ standard library dependency
2. ✅ Simpler compilation
3. ✅ Pure C code (or C with C++ comments)
4. ✅ More explicit memory management
5. ✅ Better understanding of data structures

## Trade-offs

- More manual memory management (malloc/free)
- Fixed-size arrays for some structures (MAX_NUM_COL limit)
- Manual bounds checking needed
- Bubble sort instead of std::sort (slower for large datasets, but fine for typical DB sizes)

## Next Steps

Would you like me to:
1. Complete the automatic replacement of sem_select?
2. Show you specific sections to understand before applying?
3. Test the compilation after changes?
