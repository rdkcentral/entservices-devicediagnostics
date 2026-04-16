# DeviceDiagnostics Plugin

The DeviceDiagnostics Thunder plugin provides diagnostic capabilities for RDK-based TV and Set-Top Box devices.

## Features

The plugin provides the following diagnostic APIs:

- **getAVDecoderStatus** - Gets the current status of audio/video decoder/pipeline
- **getConfiguration** - Retrieves device configuration parameters
- **getMilestones** - Returns list of diagnostic milestones
- **logMilestone** - Logs a milestone marker
- **getPreviousRebootInfo** - Retrieves detailed information about the previous device reboot

## API Documentation

See [DeviceDiagnostics.md](DeviceDiagnostics.md) for complete API documentation.

## Build

```bash
bitbake wpeframework-service-plugins
```

## Test

Example JSON-RPC request:

```bash
curl --header "Content-Type: application/json" \
     --request POST \
     --data '{"jsonrpc":"2.0","id":"3","method":"org.rdk.DeviceDiagnostics.getPreviousRebootInfo"}' \
     http://127.0.0.1:9998/jsonrpc
```

## Required Files

For **getPreviousRebootInfo** API to work, the following files must be present, both in JSON format:
- `/opt/secure/reboot/previousreboot.info` - Contains reboot metadata as a JSON object with fields:

     ```json
     {
          "timestamp": "<string>",
          "source": "<string>",
          "reason": "<string>",
          "customReason": "<string>",
          "otherReason": "<string>"
     }
     ```

- `/opt/secure/reboot/hardpower.info` - Contains last hardware power reset timestamp as a JSON object:

     ```json
     {
          "lastHardPowerReset": "<string>"
     }
     ```

## License

Apache 2.0
