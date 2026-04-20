## 1. Interface Definition

- [ ] 1.1 Define RebootInfo structure in IDeviceDiagnostic.h with six string fields (timestamp, source, reason, customReason, otherReason, lastHardPowerReset)
- [ ] 1.2 Add GetPreviousRebootInfo method signature with RebootInfo& and bool& success parameters
- [ ] 1.3 Ensure method is marked as virtual and const following Thunder conventions
- [ ] 1.4 Rebuild ThunderInterfaces to generate updated interface artifacts

## 2. Implementation

- [x] 2.1 Add GetPreviousRebootInfo method declaration to DeviceDiagnosticsImplementation.h
- [x] 2.2 Implement file reading logic to open /opt/secure/reboot/previousreboot.info
- [x] 2.3 Implement JSON parsing for previousreboot.info using Thunder Core JSON utilities
- [x] 2.4 Extract five fields (timestamp, source, reason, customReason, otherReason) from previousreboot.info
- [x] 2.5 Implement file reading logic to open /opt/secure/reboot/hardpower.info
- [x] 2.6 Implement JSON parsing for hardpower.info to extract lastHardPowerReset field
- [x] 2.7 Populate RebootInfo structure with all six fields
- [x] 2.8 Set success parameter to true on successful retrieval of primary file data
- [x] 2.9 Implement error handling for all error cases (file not found, invalid JSON, I/O errors) - return Core::ERROR_GENERAL with success=false
- [x] 2.10 Implement graceful handling when hardpower.info is missing (set lastHardPowerReset to empty, success=true)
- [x] 2.11 Handle missing fields gracefully by setting empty strings in RebootInfo structure
- [x] 2.12 Wrap all file operations and JSON parsing in try-catch blocks

## 3. Testing

- [x] 3.1 Create unit test for successful reboot info retrieval with both files present and all fields populated
- [x] 3.2 Create unit test for primaryreboot.info exists but hardpower.info missing scenario
- [x] 3.3 Create unit test for primaryreboot.info not found scenario (should return ERROR_GENERAL)
- [x] 3.4 Create unit test for invalid JSON in primaryreboot.info (should return ERROR_GENERAL)
- [x] 3.5 Create unit test for invalid JSON in hardpower.info (should still succeed with empty lastHardPowerReset)
- [x] 3.6 Create unit test for missing fields in JSON files (empty strings returned)
- [x] 3.7 Verify all error scenarios return ERROR_GENERAL with success=false

## 4. Documentation and Compliance

- [x] 4.1 Add method documentation comments following Thunder standards
- [x] 4.2 Verify compliance with Plugin.instructions.md guidelines
- [x] 4.3 Verify compliance with Pluginlifecycle.instructions.md guidelines
- [x] 4.4 Update plugin README.md with new API description and usage example
