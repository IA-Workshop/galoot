# 0256 `include-what-you-use` is not asked for

Date: 2026-11-21. Supersedes item 39 of decision 0253.

Item 39 of decision 0253 asked the owner to install `include-what-you-use`, "since header work is planned", and the coordinator's reports gave the command `sudo pacman -S --needed include-what-you-use`. When the owner asked what the tool is for, a check showed that both parts were wrong:

- No unit of plans 0043 to 0049 uses the tool. The research cycle that proposed it ranked it fifteenth, of low to medium value, noisy on template-heavy code, and in need of mapping files for the standard library. `tools/include_scan.py` already checks which headers each header may include and that the includes have no cycle. It does not check that a header includes directly what it uses, or that every include is used; that is the gap the tool would fill.
- The package is not in the machine's configured repositories (`pacman -Si` does not find it). The research notes place it in the AUR.

Decision: the tool is not asked for and no unit depends on it. If a later unit moves includes between headers and wants a one-time check of direct use, the coordinator asks again with the correct install steps and the unit that needs it.

Status: in force.
