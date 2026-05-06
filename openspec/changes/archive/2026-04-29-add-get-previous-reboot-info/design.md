## Context

The `DeviceDiagnostics` Thunder plugin currently exposes `GetConfiguration`, `GetMilestones`, `LogMilestone`, and `GetAVDecoderStatus`. It does not yet support querying the previous reboot reason.

The `SystemServices` plugin has a similar method (`getPreviousRebootInfo2`) that reads from `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info`. That method uses regex-based key-value parsing. This implementation takes a different approach: both files are expected to be in **JSON format** and are parsed using Thunder's `JsonObject::FromString()`.

The Exchange interface (`IDeviceDiagnostics`) is defined outside this repo. This design assumes that the `GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success)` method and `RebootInfo` struct are already declared in the interface file and that `Exchange::JDeviceDiagnostics` auto-generates the JSON-RPC binding.

## Goals / Non-Goals

**Goals:**
- Implement `GetPreviousRebootInfo` in `DeviceDiagnosticsImplementation.cpp`.
- Declare the method in `DeviceDiagnosticsImplementation.h`.
- Add an overloaded free function `getFileContent(std::string, std::string&)` alongside the existing list-based overload.
- Use `Core::File::Exists()` for file existence checks.
- Parse both files as JSON using `JsonObject::FromString()`.
- Return `Core::ERROR_GENERAL` if either file is missing, empty, or contains invalid JSON.
- Return `Core::ERROR_NONE` only when both files are successfully read and parsed.

**Non-Goals:**
- Modifying the Exchange interface file (assumed already updated externally).
- Writing or modifying reboot info files.
- Adding new JSON-RPC registration calls (auto-generated stubs handle this).
- Changing `DeviceDiagnostics.cpp` plugin layer (only the implementation layer changes).
- Supporting the legacy line-based `Key:Value` file format used by `SystemServices`.

## Decisions

### Decision 1: Use `Core::File::Exists()` for file existence check
**Chosen:** `Core::File(string(path)).Exists()`
**Rationale:** Consistent with Thunder framework idiom used throughout this plugin (see `GetMilestones`). Avoids mixing Thunder and non-Thunder filesystem utilities.

### Decision 2: Use free function overload `getFileContent(string, string&)` rather than a private class method
**Chosen:** Overloaded free function alongside existing `getFileContent(string, list<string>&)`.
**Rationale:** Consistent with the existing codebase pattern. The list-based overload already exists as a free function; adding a string-based overload maintains the same convention without introducing a class member just for file I/O. Uses `std::stringstream` to buffer-read the file.
**Alternative considered:** Private class method `GetFileContent` — rejected to keep consistency with the file-level helper pattern already in place.

### Decision 3: Parse files as JSON using `JsonObject::FromString()`
**Chosen:** Both info files are treated as JSON objects and parsed with `JsonObject::FromString()`.
**Rationale:** JSON is a cleaner, structured format that avoids fragile regex patterns. The Thunder framework's `JsonObject` is already used throughout the plugin and provides robust parsing with proper error detection. If `FromString()` fails, the error is reported immediately.
**Alternative considered:** Regex parsing of `Key:Value` lines (as used in `SystemServices::getPreviousRebootInfo2`) — rejected because it is fragile to whitespace/formatting variations and does not provide structured validation. JSON parsing provides implicit validation of file integrity.

### Decision 4: `hardpower.info` is non-fatal — missing or invalid yields `"Unknown"`
**Chosen:** If `hardpower.info` is absent, unreadable, contains invalid JSON, or lacks the `lastHardPowerReset` key, `rebootInfo.lastHardPowerReset` is set to `"Unknown"` and the API returns `Core::ERROR_NONE`.
**Rationale:** Some platforms may not write `hardpower.info` on every reboot (e.g., soft reboots). Treating a missing hard-power file as a fatal error would cause the API to always fail on such platforms, providing no data at all to diagnostics consumers. Returning a partial response with `"Unknown"` is more useful than a complete failure. The primary reboot info (`previousreboot.info`) is the required source of truth; `hardpower.info` supplements it.
**Alternative considered:** Treat both files as required; missing `hardpower.info` returns `ERROR_GENERAL` — rejected because it breaks the API on platforms where `hardpower.info` is not written for every reboot type.

### Decision 5: Error handling strategy
**Chosen:**
- Return `Core::ERROR_GENERAL` (and `success = false`) when:
  - `previousreboot.info` does not exist
  - `previousreboot.info` is empty
  - `previousreboot.info` fails JSON parsing
- Set `lastHardPowerReset = "Unknown"` and continue (return `Core::ERROR_NONE`) when:
  - `hardpower.info` is missing or unreadable
  - `hardpower.info` fails JSON parsing
  - `hardpower.info` does not contain the `lastHardPowerReset` key
  - `lastHardPowerReset` value is empty or `"null"`
- Return `Core::ERROR_NONE` (and `success = true`) when `previousreboot.info` is successfully read and parsed, regardless of `hardpower.info` state.

## Implementation Summary

### Files Modified

| File | Change |
|------|--------|
| `plugin/DeviceDiagnosticsImplementation.h` | Added `GetPreviousRebootInfo(RebootInfo&, bool&)` public override |
| `plugin/DeviceDiagnosticsImplementation.cpp` | Added `#include <sstream>`, file path constants, `getFileContent(string, string&)` overload, and `GetPreviousRebootInfo` implementation |
| `Tests/L1Tests/tests/test_DeviceDiagnostics.cpp` | Added 7 L1 test cases covering success, missing files, invalid JSON, empty files, and partial field scenarios |

### `GetPreviousRebootInfo` Logic
```
1. Check Core::File(PREVIOUS_REBOOT_INFO_FILE).Exists()
   - If false: success = false; return Core::ERROR_GENERAL

2. Call getFileContent(PREVIOUS_REBOOT_INFO_FILE, rebootInfoContent)
   - If fails or content empty: success = false; return Core::ERROR_GENERAL

3. JsonObject::FromString(rebootInfoContent)
   - If fails: success = false; return Core::ERROR_GENERAL
   - Extract: timestamp, source, reason, customReason, otherReason

4. Call getFileContent(HARD_POWER_INFO_FILE, hardPowerInfo)
   Parse with JsonObject::FromString() and check HasLabel("lastHardPowerReset")
   - If any of: getFileContent fails, JSON parse fails, or key missing:
       rebootInfo.lastHardPowerReset = "Unknown" (non-fatal; continue)
   - Else:
       value = hardPowerInfoJson["lastHardPowerReset"].String()
       rebootInfo.lastHardPowerReset = (value.empty() || value == "null") ? "Unknown" : value

5. success = true; return Core::ERROR_NONE
```

### `getFileContent(string, string&)` Logic
```
1. Open std::ifstream for fileName
   - If fails to open: return false
2. Use std::stringstream to buffer-read entire contents
3. Copy to fileContent string; close file; return true
```

## Risks / Trade-offs

- **Risk: JSON format dependency** → Both files must be in JSON. If the OS writes these files in a legacy key-value format, parsing will fail.
  *Mitigation*: Document the expected file format clearly. The existing `SystemServices` implementation uses the key-value format; platform owners must ensure `DeviceDiagnostics` targets devices whose reboot files are in JSON format.

- **Risk: `hardpower.info` unavailable on some reboots** → Some platforms may not write `hardpower.info` on soft reboots without a power cycle.
  *Trade-off accepted*: `hardpower.info` is treated as non-fatal. Missing or invalid file results in `lastHardPowerReset = "Unknown"` while `previousreboot.info` data is still returned. This is a deliberate design choice to maximise API availability across platforms.

- **Risk: Interface not yet updated** → If `RebootInfo` / `GetPreviousRebootInfo` are not yet in the Exchange interface, the build will fail.
  *Mitigation*: Coordinate interface changes before merging.

## Migration Plan

1. Ensure Exchange interface (`IDeviceDiagnostics`) and `JDeviceDiagnostics` stub are updated externally.
2. Ensure both platform reboot info files are written in JSON format.
3. Build, verify L1 tests pass.

## Open Questions

- Are both reboot files confirmed to be in JSON format on all target platforms?
