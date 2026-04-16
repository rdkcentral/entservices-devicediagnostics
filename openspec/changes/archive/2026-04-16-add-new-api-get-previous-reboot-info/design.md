## Context

The DeviceDiagnostics plugin currently lacks an API to retrieve the previous reboot reason and related metadata. Apps and diagnostic systems need this information to send accurate diagnostics to endpoints like SIFT. The new API will standardize access to this data, reading from files written by the system at reboot.

## Goals / Non-Goals

**Goals:**
- Provide a Thunder API (`getPreviousRebootInfo`) in DeviceDiagnostics to return previous reboot information.
- Ensure the API reads from `/opt/secure/reboot/previousreboot.info` and `/opt/secure/reboot/hardpower.info`.
- Return all required fields in a structured response, matching the specified JSON format.
- Return `Core::ERROR_NONE` on success, `Core::ERROR_GENERAL` on error.

**Non-Goals:**
- Does not change how reboot info is written to the files (assumes system already writes them).
- Does not add new reboot reasons or modify reboot logic elsewhere in the system.
- Does not provide real-time or streaming updates; only point-in-time retrieval.

## Decisions

- **API Signature:**
  - `Core::hresult GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success);`
  - `RebootInfo` struct will contain: timestamp, source, reason, customReason, otherReason, lastHardPowerReset.
- **File Parsing:**
  - `/opt/secure/reboot/previousreboot.info` is expected to be a JSON object with fields: timestamp, source, reason, customReason, otherReason.
  - `/opt/secure/reboot/hardpower.info` is expected to be a JSON object with field: lastHardPowerReset.
  - If either file is missing, unreadable, or contains invalid JSON, the API will return `Core::ERROR_GENERAL` and set `success` to false.
  - If fields are missing in the JSON, the corresponding output fields will be empty strings.
- **Reference Implementation:**
  - Use the logic from `SystemServices::getPreviousRebootInfo2()` as a reference for file parsing and error handling.
- **Response Format:**
  - The API will return a JSON object with a `rebootInfo` dictionary and a `success` boolean, as specified.

## Risks / Trade-offs

- [Risk] Files may be missing or corrupted → [Mitigation] Return `success: false` and `Core::ERROR_GENERAL` if parsing fails.
- [Risk] File format changes in the future → [Mitigation] Document file format expectations and validate fields before returning.
- [Risk] Potential for race conditions if files are being written while read → [Mitigation] Use file locks if available, otherwise document as a known limitation.
