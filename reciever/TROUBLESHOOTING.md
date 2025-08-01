# Troubleshooting Google Sheets Integration

## Current Issue: Device showing as number, missing unit_flow, esp32_ip, timestamps

### Step 1: Update Google Apps Script

Replace your Google Apps Script code with the updated version in `google_apps_script.js`. The new version includes:

- Explicit type conversion (String(), Number())
- Debug logging to help identify issues
- Better error handling

### Step 2: Test the Connection

1. Upload the updated ESP32 code
2. Open Serial Monitor
3. The ESP32 will automatically test the Google Sheets connection on startup
4. Look for the test data in your Google Sheets

### Step 3: Check Google Apps Script Logs

1. Go to Google Apps Script (script.google.com)
2. Open your project
3. Click on "Executions" in the left sidebar
4. Look for recent executions and check for errors
5. Click on an execution to see detailed logs

### Step 4: Verify Data Format

The ESP32 should send JSON like this:

```json
{
  "device": "flowmeter",
  "flow_rate": 2.45,
  "total_volume": 15.234,
  "unit_flow": "L/min",
  "unit_volume": "L",
  "received_timestamp": 123456,
  "esp32_ip": "192.168.1.100"
}
```

### Step 5: Common Issues and Solutions

#### Device showing as number

- **Cause**: Device field not being sent as string
- **Solution**: Updated ESP32 code now explicitly sends "flowmeter" as string

#### Missing unit_flow, esp32_ip, timestamps

- **Cause**: Google Apps Script not properly extracting these fields
- **Solution**: Updated script with explicit type conversion

#### HTTP Error Codes

- **-11**: Read timeout - Most common issue. Network connectivity problem or Google Sheets taking too long to respond
- **-1**: Connection refused - Can't connect to Google servers
- **-4**: Not connected - WiFi connection lost
- **-5**: Connection lost during request
- **302**: Redirect - Apps Script URL might be incorrect
- **403**: Permission denied - Check Apps Script deployment permissions
- **404**: Not found - Verify the Apps Script URL

#### Fixing HTTP Error -11 (Timeout)

This is the most common error. Try these solutions:

1. **Check Internet Connection**: Make sure your WiFi has internet access
2. **Verify Google Script URL**: Ensure the URL is correct and the script is deployed
3. **Re-deploy Apps Script**: Create a new deployment with a fresh URL
4. **Check Network Firewall**: Some networks block outgoing HTTPS requests
5. **Try Different WiFi**: Test on a different network
6. **Increase Timeout**: The code now uses 15-second timeouts
7. **Use Retry Logic**: The code now automatically retries failed requests

### Step 6: Manual Testing

You can test the Google Apps Script manually:

1. In Google Apps Script, select the `testScript` function
2. Click "Run"
3. Check if test data appears in your sheet

### Step 7: Re-deploy Apps Script

If changes aren't taking effect:

1. Go to Google Apps Script
2. Click "Deploy" → "Manage deployments"
3. Click the edit icon (pencil) next to your deployment
4. Change version to "New version"
5. Click "Deploy"
6. Update the URL in your ESP32 code if it changed

### Debug Serial Output

Look for this pattern in Serial Monitor:

```
Testing Google Sheets connection...
Debug - Individual values:
Device: flowmeter
Flow Rate: 1.23
Total Volume: 4.56
ESP32 IP: 192.168.1.100
Timestamp: 12345
Sending to Google Sheets: {"device":"flowmeter","flow_rate":1.23,"total_volume":4.56,"unit_flow":"L/min","unit_volume":"L","received_timestamp":12345,"esp32_ip":"192.168.1.100"}
HTTP Response Code: 200
Server Response: {"status":"success","message":"Flowmeter data added successfully",...}
```

### Expected Google Sheets Format

| Timestamp           | Device    | Flow Rate | Flow Unit | Total Volume | Volume Unit | ESP32 Timestamp | ESP32 IP      |
| ------------------- | --------- | --------- | --------- | ------------ | ----------- | --------------- | ------------- |
| 2025-08-01 10:30:15 | flowmeter | 1.23      | L/min     | 4.56         | L           | 12345           | 192.168.1.100 |

If you still see issues after following these steps, check the Google Apps Script execution logs for detailed error messages.
