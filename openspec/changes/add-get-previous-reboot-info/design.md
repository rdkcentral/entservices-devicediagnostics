## Context

The `DeviceDiagnostics` Thunder plugin currently exposes `GetConfiguration`, `GetMilestones`, `LogMilestone`, and `GetAVDecoderStatus`. It does not yet support querying the previous reboot reason.

The `SystemServices` plugin has a similar method (`getPreviousRebootInfo2`) that reads from `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info`. That logic serves as the reference implementation for this change.

The Exchange interface (`IDeviceDiagnostics`) is defined outside this repo. This design assumes that the `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)` method and `RebootInfo` struct are already declared in the interface file and that `Exchange::JDeviceDiagnostics` auto-generates the JSON-RPC binding.

## Goals / Non-Goals

**Goals:**
- Implement `GetPreviousRebootInfo` in `DeviceDiagnosticsImplementation.cpp`.
- Declare the method in `DeviceDiagnosticsImplementation.h`.
- Add a private `GetFileContent(const string& filePath, string& content)` helper to read file contents into a string.
- Use `Core::File` for file existence checks (not `Utils::fileExists` or `std::ifstream` directly).
- Return `Core::ERROR_NONE` on success and `Core::ERROR_GENERAL` on any failure.
- Parse fields (`timestamp`, `source`, `reason`, `customReason`, `otherReason`) from `/opt/secure/reboot/previousreboot.info`.
- Read `lastHardPowerReset` from `/opt/secure/reboot/hardpower.info`.

**Non-Goals:**
- Modifying the Exchange interface file (assumed already updated externally).
- Writing or modifying reboot info files.
- Adding new JSON-RPC registration calls (auto-generated stubs handle this).
- Changing `DeviceDiagnostics.cpp` plugin layer (only the implementation layer changes).

## Decisions

### Decision 1: Use `Core::File` for file existence check
**Chosen:** `Core::File file(path); if (file.Exists()) { ... }`  
**Rationale:** Consistent with Thunder framework idiom for file operations. Avoids the utility helper (`Utils::fileExists`) which has a dependency on non-core utilities.  
**Alternative considered:** `std::ifstream` — rejected because it conflates existence check with read and does not communicate filesystem errors explicitly.

### Decision 2: Implement a private `GetFileContent` helper
**Chosen:** `bool GetFileContent(const string& filePath, string& content)` reads the full file into a string using `std::ifstream`.  
**Rationale:** Keeps `GetPreviousRebootInfo` focused on parsing logic. Can be reused for both files. Returns `false` if file cannot be opened.

### Decision 3: Use `std::regex` for field parsing (reference pattern from SystemServices)
**Chosen:** Parse key-value pairs using `std::regex_search` with named patterns such as `(?:PreviousRebootTime:)([^\n]+)`.  
**Rationale:** The `SystemServices::getPreviousRebootInfo2` reference implementation uses this approach successfully. Line-based parsing is an alternative but regex is already validated in the codebase.  
**Alternative considered:** Line-by-line split + `find(':')` — simpler but more brittle for multi-line edge cases.

### Decision 4: `lastHardPowerReset` parsed with `getline` from hardpower.info
**Chosen:** Read the entire first meaningful line from `/opt/secure/reboot/hardpower.info` as the `lastHardPowerReset` value.  
**Rationale:** The hardpower.info file contains a single date-time string. Simple `getline` is sufficient.

### Decision 5: Error handling strategy
**Chosen:** Return `Core::ERROR_GENERAL` (and set `success = false`) if the primary reboot info file is missing or unreadable. Return `Core::ERROR_NONE` (and set `success = true`) if the primary file is read successfully, even if hardpower.info is absent.  
**Rationale:** `lastHardPowerReset` is supplementary information; its absence should not block the primary response.

## Implementation Approach

### Files Modified

| File | Change |
|------|--------|
| `plugin/DeviceDiagnosticsImplementation.h` | Add `GetPreviousRebootInfo` declaration and `GetFileContent` private helper declaration |
| `plugin/DeviceDiagnosticsImplementation.cpp` | Implement `GetPreviousRebootInfo` and `GetFileContent` |

### `GetPreviousRebootInfo` Logic (pseudocode)
```
1. Declare file path constants:
   PREVIOUS_REBOOT_INFO_FILE = "/opt/secure/reboot/previousreboot.info"
   HARD_POWER_INFO_FILE      = "/opt/secure/reboot/hardpower.info"

2. Use Core::File to check PREVIOUS_REBOOT_INFO_FILE exists.
   - If missing: success = false; return Core::ERROR_GENERAL

3. Call GetFileContent(PREVIOUS_REBOOT_INFO_FILE, content).
   - If fails: success = false; return Core::ERROR_GENERAL

4. Use std::regex_search to extract:
   - PreviousRebootTime:       → rebootInfo.timestamp
   - PreviousRebootReason:     → rebootInfo.reason
   - PreviousRebootInitiatedBy: → rebootInfo.source
   - PreviousCustomReason:     → rebootInfo.customReason
   - PreviousOtherReason:      → rebootInfo.otherReason

5. Use Core::File to check HARD_POWER_INFO_FILE exists.
   - If exists: call GetFileContent and parse first non-empty line
     → rebootInfo.lastHardPowerReset

6. success = true; return Core::ERROR_NONE
```

### `GetFileContent` Logic (pseudocode)
```
1. Open std::ifstream for filePath.
   - If fails: return false
2. Read entire contents into string via std::istreambuf_iterator.
3. Close file. Return true.
```

## Risks / Trade-offs

- **Risk: Regex parsing fragility** → The regex patterns are tied to the exact key names in the file format. If the file format changes (different key names), the parser silently returns empty strings.  
  *Mitigation*: Empty fields default to `""`, so behavior degrades gracefully. The file format is an OS-level convention unlikely to change.

- **Risk: Interface not yet updated** → This implementation assumes `GetPreviousRebootInfo` and `RebootInfo` are already in the Exchange interface. If they are not, the build will fail with missing symbol errors.  
  *Mitigation*: Coordinate interface changes before merging.

- **Risk: Large reboot info files** → Reading the entire file with `GetFileContent` could be inefficient for unexpectedly large files.  
  *Mitigation*: Reboot info files are typically very small (~1 KB). No streaming needed.

## Migration Plan

1. Ensure Exchange interface (`IDeviceDiagnostics`) and `JDeviceDiagnostics` stub are updated externally with `GetPreviousRebootInfo` and `RebootInfo`.
2. Implement `GetFileContent` and `GetPreviousRebootInfo` in `DeviceDiagnosticsImplementation.cpp`.
3. Add declaration to `DeviceDiagnosticsImplementation.h`.
4. Build and verify JSON-RPC stub auto-registration picks up the new method.
5. Run L1 unit tests.

## Open Questions

- Are the exact file paths (`/opt/secure/reboot/previousreboot.info`, `/opt/secure/reboot/hardpower.info`) confirmed for all target platforms, or are they platform-specific?
- Should missing individual fields (e.g., `reason`) cause `success: false`, or should partial data always return `success: true`?
