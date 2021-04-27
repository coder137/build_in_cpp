# Include directory vs Header files 

From our `target.fbs` schema we can see that we track both `include_dirs` and `header_files`

```
header_files:[path]
include_dirs:[string]
```

# Short explanation

- **header_files** are used to track rebuilds when specific header files are added, removed and updated.
  - This lets use easily track when header files are updated with user input.
- **include_directories** are used to track rebuilds when include directory is added or removed.
  - This is required by our compiler with the `-I` flag.

# Long explanation

```
D:.
│   target.h
│
└───internal
        fbs_loader.h
        path.h
        util.h
```

Depending on the above tree we might choose to just add `-I.` to our build.

We can now `#include` our header files in this format

```c
#include "target.h"
#include "internal/fbs_loader.h"
#include "internal/path.h"
#include "internal/util.h"
```

However when just adding `-I.` to our build we cannot accurately track header files for rebuilds i.e specifically the header files present inside the **internal** folder.

> Recursively parsing the tree and tracking header files over `-I.` (in this case) would be computationally expensive.

> We might potentially track header files that are not explicitly required by our project.

This can be solved by tracking individual header files per target for rebuilds.

# Future work

- Add header file globbing APIs to avoid manually adding each individual header file.
- Add globbing over `Add/Append IncludeDir` API to glob header files pertaining to the directory added.
- Recursive globbing should not be allowed
