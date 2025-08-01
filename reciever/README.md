# LoRa Receiver with Google Sheets Integration

This project receives LoRa data containing flow rate and volume measurements and automatically sends them to Google Sheets.

## Expected LoRa Data Format

The code expects LoRa data in the following format:

````
# LoRa Receiver with Google Sheets Integration

This project receives LoRa data containing flow rate and volume measurements in JSON format and automatically sends them to Google Sheets.

## Expected LoRa Data Format
The code expects LoRa data in the following JSON format:
```json
{
  "device":"flowmeter",
  "timestamp":12345678,
  "flow_rate":2.45,
  "total_volume":15.234,
  "unit_flow":"L/min",
  "unit_volume":"L"
}
````

## Setup Instructions

### 1. Hardware Setup

- ESP32 development board
- LoRa module connected to pins:
  - SS: Pin 5
  - RST: Pin 14
  - DIO0: Pin 2

### 2. Google Sheets Setup

1. Create a new Google Spreadsheet
2. Go to Google Apps Script (script.google.com)
3. Create a new project
4. Replace the default code with the content from `google_apps_script.js`
5. Deploy the script as a web app:
   - Click "Deploy" > "New deployment"
   - Choose "Web app" as the type
   - Set execute as "Me"
   - Set access to "Anyone"
   - Click "Deploy"
   - Copy the provided web app URL

### 3. ESP32 Code Configuration

Update the following in `main.cpp`:

```cpp
// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";           // Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD";   // Replace with your WiFi password

// Google Apps Script Web App URL
const char* googleScriptURL = "YOUR_GOOGLE_SCRIPT_URL";  // Replace with your web app URL
```

### 4. Google Spreadsheet Setup (Optional)

Run the `setupSpreadsheet()` function in Google Apps Script to automatically create headers:

1. In the Apps Script editor, select the `setupSpreadsheet` function
2. Click the "Run" button
3. This will create headers for all the flowmeter data fields

## How It Works

1. The ESP32 connects to WiFi on startup
2. Initializes the LoRa module
3. Listens for incoming LoRa packets
4. When a packet is received:
   - Parses the JSON data to extract flowmeter information
   - Validates the device type and required fields
   - Sends the parsed data to Google Sheets via HTTP POST
   - Logs the results to Serial monitor

## Data Format in Google Sheets

| Timestamp           | Device    | Flow Rate | Flow Unit | Total Volume | Volume Unit | ESP32 Timestamp | ESP32 IP      |
| ------------------- | --------- | --------- | --------- | ------------ | ----------- | --------------- | ------------- |
| 2025-08-01 10:30:15 | flowmeter | 2.45      | L/min     | 15.234       | L           | 12345678        | 192.168.1.100 |
| 2025-08-01 10:30:30 | flowmeter | 2.50      | L/min     | 15.489       | L           | 12345693        | 192.168.1.100 |

## JSON Parsing Features

- **Device Validation**: Ensures the device type is "flowmeter"
- **Required Field Check**: Validates that essential fields (device, flow_rate, total_volume) are present
- **Flexible Units**: Supports different units with fallback to default values
- **Error Handling**: Provides detailed error messages for JSON parsing issues
- **Additional Data**: Captures timestamp and units from the original flowmeter data

## Troubleshooting

1. **WiFi Connection Issues**: Check SSID and password
2. **LoRa Not Working**: Verify pin connections and frequency (433MHz)
3. **Google Sheets Not Updating**:
   - Verify the web app URL is correct
   - Check that the script is deployed with proper permissions
   - Look at Serial monitor for HTTP response codes
4. **JSON Parsing Errors**:
   - Ensure LoRa data is valid JSON format
   - Check that required fields are present
   - Verify device type is "flowmeter"
5. **Invalid Device Type**: Make sure the JSON contains `"device":"flowmeter"`

## Serial Monitor Output Example

```
LoRa Receiver with Google Sheets Integration
Connecting to WiFi....
Connected to WiFi. IP address: 192.168.1.100
LoRa Initializing OK!
Received packet: {"device":"flowmeter","timestamp":12345678,"flow_rate":2.45,"total_volume":15.234,"unit_flow":"L/min","unit_volume":"L"}
RSSI: -45
Parsed JSON data:
Device: flowmeter
Timestamp: 12345678
Flow Rate: 2.45 L/min
Total Volume: 15.234 L
Sending to Google Sheets: {"device":"flowmeter","flow_rate":2.45,"total_volume":15.234,"unit_flow":"L/min","unit_volume":"L","received_timestamp":123456,"esp32_ip":"192.168.1.100"}
HTTP Response: 200
Response: {"status":"success","message":"Flowmeter data added successfully"}
```

````

## Setup Instructions

### 1. Hardware Setup

- ESP32 development board
- LoRa module connected to pins:
  - SS: Pin 5
  - RST: Pin 14
  - DIO0: Pin 2

### 2. Google Sheets Setup

1. Create a new Google Spreadsheet
2. Go to Google Apps Script (script.google.com)
3. Create a new project
4. Replace the default code with the content from `google_apps_script.js`
5. Deploy the script as a web app:
   - Click "Deploy" > "New deployment"
   - Choose "Web app" as the type
   - Set execute as "Me"
   - Set access to "Anyone"
   - Click "Deploy"
   - Copy the provided web app URL

### 3. ESP32 Code Configuration

Update the following in `main.cpp`:

```cpp
// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";           // Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD";   // Replace with your WiFi password

// Google Apps Script Web App URL
const char* googleScriptURL = "YOUR_GOOGLE_SCRIPT_URL";  // Replace with your web app URL
````

### 4. Google Spreadsheet Setup (Optional)

Run the `setupSpreadsheet()` function in Google Apps Script to automatically create headers:

1. In the Apps Script editor, select the `setupSpreadsheet` function
2. Click the "Run" button
3. This will create headers: "Timestamp", "Flow Rate (L/min)", "Volume (L)"

## How It Works

1. The ESP32 connects to WiFi on startup
2. Initializes the LoRa module
3. Listens for incoming LoRa packets
4. When a packet is received:
   - Parses the flow rate and volume from the data
   - Sends the parsed data to Google Sheets via HTTP POST
   - Logs the results to Serial monitor

## Data Format in Google Sheets

| Timestamp           | Flow Rate (L/min) | Volume (L) |
| ------------------- | ----------------- | ---------- |
| 2025-08-01 10:30:15 | 1.25              | 5.50       |
| 2025-08-01 10:30:30 | 1.30              | 5.85       |

## Troubleshooting

1. **WiFi Connection Issues**: Check SSID and password
2. **LoRa Not Working**: Verify pin connections and frequency (433MHz)
3. **Google Sheets Not Updating**:
   - Verify the web app URL is correct
   - Check that the script is deployed with proper permissions
   - Look at Serial monitor for HTTP response codes
4. **Parsing Errors**: Ensure LoRa data matches the expected format exactly

## Serial Monitor Output Example

```
LoRa Receiver with Google Sheets Integration
Connecting to WiFi....
Connected to WiFi. IP address: 192.168.1.100
LoRa Initializing OK!
Received packet 'Flow: 1.25 L/min, Volume: 5.50 L' with RSSI -45
Parsed data:
Flow Rate: 1.25 L/min
Volume: 5.50 L
Sending to Google Sheets: {"flow":1.25,"volume":5.5,"timestamp":12345}
HTTP Response: 200
Response: {"status":"success","message":"Data added successfully"}
```
