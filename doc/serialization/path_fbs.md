# Understanding `path.fbs`

See [path.fbs](../../buildcc/schema/path.fbs)

## Current reasoning

- From our software architecture in the project README we can see that a Target needs to verify the physical presence of the following files.
    - Source files
    - Header files
    - PCH files
    - Lib dependencies
- Additionally we also track
    - User defined compile dep files
    - User defined link dep files

For efficient rebuilds where physical presence of a **set of files** need to be verified we check the **previous loaded set** with the **current input set**

The following states can be observed:
- FILE_REMOVED: **current_set does not have a filepath that is present in loaded_set**
- FILE_ADDED: **loaded_set does not have a filepath that is present in current_set**
- FILE_UPDATED: **loaded_set::file::stored_timestamp is older than current_set::file::stored_timestamp**

For the **FILE_REMOVED** and **FILE_ADDED** state we check `schema::internal::Path::pathname` for any differences

For the **FILE_UPDATED** state we check `schema::internal::Path::last_write_timestamp` for any differences

## Future Scope

Our rebuilds focus on 3 states
- FILE_REMOVED
- FILE_ADDED
- FILE_UPDATED

For a future scope we can think about efficient rebuilds for FILE_UPDATED state

- Timestamp to rebuild
    - Faster to compute
    - Might cause unnecessary rebuilds when a file is saved but diff does not change
- File hash to rebuild
    - Slower to compute
    - More efficient since it would only rebuild / be marked dirty if the file contains diffs from the previous version

In the future instead of having

```json
{
    "pathname": "string",
    "last_write_timestamp": "uint64",
}
```

we can have 

```json
{
    "pathname": "string",
    "rebuild_strategy": "string",
}
```

Now our **FILE_UPDATED** state could contain both
- Timestamp strategy
- File Hash strategy 

encompassed into the same structure
