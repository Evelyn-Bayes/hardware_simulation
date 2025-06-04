# CPU Simulator Style Guide

## Formatting
All code should adhere to the clang-format style as defined in the `.clang-format` file in the root directory of the project.

## Naming Conventions

| Element       | Style          | Example               |
|---------------|----------------|-----------------------|
| Functions     | `snake_case`   | `execute_next`        |
| Variables     | `snake_case`   | `program_counter`     |
| Constants     | `UPPER_CASE`   | `PROGRAM_COUNTER_MAX` |
| Structs/Types | `PascalCase`   | `CpuState`            |

## Comments

```c
/*
 * Multi line comment
 */
<code>
---

/* Single line comment */
<code>
---

<code> // End of line comment
---

/*********************************
 * Code block description        *
 *********************************/

<code>
---

/*>>>>> Code block divider >>>>>*/

<code>
```
