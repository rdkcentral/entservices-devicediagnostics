## 1. Interface Change (entservices-apis repo)

- [ ] 1.1 In `IDeviceDiagnostics.h`, add `@json` annotation to the existing `RebootInfo` struct to enable auto-stub generation for all its fields
- [ ] 1.2 Add the new method declaration to `Exchange::IDeviceDiagnostics`:
  ```cpp
  virtual Core::hresult GetPreviousRebootInfo(RebootInfo& rebootInfo /* @out */, bool& success /* @out */) = 0;
  ```
- [ ] 1.3 Regenerate `Exchange::JDeviceDiagnostics` (JSON-RPC auto-generated stubs) after the interface change and commit the updated header
- [ ] 1.4 Open and merge the `entservices-apis` PR before proceeding to implementation tasks

## 2. Implementation — DeviceDiagnosticsImplementation

- [x] 2.1 In `plugin/DeviceDiagnosticsImplementation.h`, add the `GetPreviousRebootInfo` method declaration:
  ```cpp
  Core::hresult GetPreviousRebootInfo(RebootInfo& rebootInfo, bool& success) override;
  ```
- [x] 2.2 In `plugin/DeviceDiagnosticsImplementation.cpp`, define file path constants:
  ```cpp
  #define PREVIOUS_REBOOT_INFO_FILE  "/opt/secure/reboot/previousreboot.info"
  #define HARD_POWER_RESET_FILE      "/opt/secure/reboot/hardpower.info"
  ```
- [x] 2.3 Implement `DeviceDiagnosticsImplementation::GetPreviousRebootInfo()`:
  - Open `PREVIOUS_REBOOT_INFO_FILE`; if absent/unreadable, set `success = false` and return `Core::ERROR_GENERAL`
  - Parse each `key=value` line; map `reboot_timestamp` → `rebootInfo.timestamp`, `reboot_source` → `rebootInfo.source`, `reboot_reason` → `rebootInfo.reason`, `reboot_custom_reason` → `rebootInfo.customReason`, `reboot_other_reason` → `rebootInfo.otherReason`
  - Open `HARD_POWER_RESET_FILE`; if present, read the first trimmed line into `rebootInfo.lastHardPowerReset`; if absent, leave as empty string (do not affect `success`)
  - Set `success = true` and return `Core::ERROR_NONE`

## 3. Spec Update — DeviceDiagnostics_Spec.md

- [x] 3.1 Add `getPreviousRebootInfo` to the JSON-RPC methods table in `## External Interfaces`
- [x] 3.2 Add full JSON request / response example for `getPreviousRebootInfo` (matching the format already used for other methods)
- [x] 3.3 Add `GetPreviousRebootInfo` to the COM-RPC interface methods table with signature, direction (`out`), and description
- [x] 3.4 Add runtime file dependencies (`/opt/secure/reboot/previousreboot.info`, `/opt/secure/reboot/hardpower.info`) to the External System Dependencies table
- [x] 3.5 Add error row for `GetPreviousRebootInfo` (file missing → `Core::ERROR_GENERAL` / `success: false`) to the Error Return Summary table
- [x] 3.6 Resolve OQ-01: note that `RebootInfo` is now exposed via `getPreviousRebootInfo` and remove or close the open query

## 4. Unit Tests — L1

- [x] 4.1 Add `TEST_F(DeviceDiagnosticsTest, getPreviousRebootInfo_success)`:
  - Create a temporary `previousreboot.info` fixture with all five key=value fields
  - Create a temporary `hardpower.info` fixture
  - Mock `GetPreviousRebootInfo` on `DeviceDiagnosticsMock` to return `Core::ERROR_NONE` with populated `RebootInfo`
  - Assert JSON-RPC response contains `rebootInfo` object and `"success": true`
- [x] 4.2 Add `TEST_F(DeviceDiagnosticsTest, getPreviousRebootInfo_fileNotFound)`:
  - Mock `GetPreviousRebootInfo` to return `Core::ERROR_GENERAL` with `success = false`
  - Assert JSON-RPC response contains `"success": false`
- [x] 4.3 Add `TEST_F(DeviceDiagnosticsTest, getPreviousRebootInfo_hardpowerMissing)`:
  - Mock `GetPreviousRebootInfo` returning `Core::ERROR_NONE`, `success = true`, and empty `lastHardPowerReset`
  - Assert `"lastHardPowerReset": ""` and `"success": true` in response

## 5. Integration Tests — L2

- [x] 5.1 Add `TEST_F(DeviceDiagnosticsL2Test, GetPreviousRebootInfo_JSONRPC)`:
  - Create `/tmp/previousreboot.info` and `/tmp/hardpower.info` test fixtures (use environment variable or config override for file paths in test build, or create the actual paths with test data)
  - Invoke `org.rdk.DeviceDiagnostics.getPreviousRebootInfo` via JSON-RPC
  - Assert full `rebootInfo` object in response with all six fields populated
- [x] 5.2 Add `TEST_F(DeviceDiagnosticsL2Test, GetPreviousRebootInfo_COMRPC)`:
  - Call `Exchange::IDeviceDiagnostics::GetPreviousRebootInfo` via COM-RPC directly
  - Assert `success == true` and all `RebootInfo` fields match expected fixture values

## 6. Coverage Report Update

- [x] 6.1 Add `Plugin::DeviceDiagnosticsImplementation::GetPreviousRebootInfo` to `## Covered Code` in `specs/DeviceDiagnostics_Spec.md`
- [x] 6.2 Re-run openspec-coverage and update `specs/spec_coverage.md` with the revised score
