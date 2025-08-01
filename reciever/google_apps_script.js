function doPost(e) {
  try {
    console.log('Raw request body:', e.postData.contents);
    console.log('Content type:', e.postData.type);
    
    var data = JSON.parse(e.postData.contents);
    
    console.log('Parsed JSON data:', JSON.stringify(data));
    console.log('Data keys:', Object.keys(data));
    
    var sheet = SpreadsheetApp.getActiveSheet();
    
    var serverTimestamp = new Date();
    
    var senderTimestamp = Number(data.timestamp || -999);
    var avgFlowRate = Number(data.avg_flow_rate || -999);
    var stdFlowRate = Number(data.std_flow_rate || -999);
    
    var dataQuality = 'Good';
    if (isNaN(avgFlowRate) || isNaN(stdFlowRate)) {
      dataQuality = 'Invalid_Numbers';
    } else if (avgFlowRate < 0 || stdFlowRate < 0) {
      dataQuality = 'Negative_Values';
    } else if (avgFlowRate > 100) {
      dataQuality = 'High_Flow_Warning';
    } else if (stdFlowRate > avgFlowRate && avgFlowRate > 0) {
      dataQuality = 'High_Variation';
    }
    
    console.log('Individual field extraction:');
    console.log('timestamp:', typeof data.timestamp, '=', data.timestamp, '-> extracted as:', senderTimestamp);
    console.log('avg_flow_rate:', typeof data.avg_flow_rate, '=', data.avg_flow_rate, '-> extracted as:', avgFlowRate);
    console.log('std_flow_rate:', typeof data.std_flow_rate, '=', data.std_flow_rate, '-> extracted as:', stdFlowRate);
    console.log('data_quality:', dataQuality);
    
    sheet.appendRow([
      serverTimestamp,
      senderTimestamp,
      avgFlowRate,
      stdFlowRate,
      dataQuality,
      'flowmeter'
    ]);
    
    return ContentService
      .createTextOutput(JSON.stringify({
        'status': 'success',
        'message': 'Flowmeter data added successfully',
        'debug': {
          'received_keys': Object.keys(data),
          'sender_timestamp': senderTimestamp,
          'avg_flow_rate': avgFlowRate,
          'std_flow_rate': stdFlowRate,
          'data_quality': dataQuality
        },
        'server_timestamp': serverTimestamp
      }))
      .setMimeType(ContentService.MimeType.JSON);
      
  } catch (error) {
    console.log('Error occurred:', error.toString());
    console.log('Error stack:', error.stack);
    console.log('Raw request data:', e.postData ? e.postData.contents : 'No postData');
    
    var debugInfo = {
      'error': error.toString(),
      'hasPostData': !!e.postData,
      'postDataType': e.postData ? e.postData.type : 'none',
      'postDataLength': e.postData ? e.postData.contents.length : 0,
      'rawData': e.postData ? e.postData.contents : 'none'
    };
    
    return ContentService
      .createTextOutput(JSON.stringify({
        'status': 'error',
        'message': error.toString(),
        'debug': debugInfo
      }))
      .setMimeType(ContentService.MimeType.JSON);
  }
}

function doGet(e) {
  return ContentService
    .createTextOutput('LoRa Flowmeter Data Receiver is running - Expects: timestamp, avg_flow_rate, std_flow_rate')
    .setMimeType(ContentService.MimeType.TEXT);
}

function setupSpreadsheet() {
  var sheet = SpreadsheetApp.getActiveSheet();
  
  sheet.clear();
  sheet.appendRow([
    'Server Timestamp',
    'Sender Timestamp',
    'Avg Flow Rate (L/min)',
    'Std Flow Rate (L/min)',
    'Data Quality',
    'Device Type'
  ]);
  
  var headerRange = sheet.getRange(1, 1, 1, 6);
  headerRange.setFontWeight('bold');
  headerRange.setBackground('#4285f4');
  headerRange.setFontColor('white');
  
  sheet.setColumnWidth(1, 150);
  sheet.setColumnWidth(2, 120);
  sheet.setColumnWidth(3, 130);
  sheet.setColumnWidth(4, 130);
  sheet.setColumnWidth(5, 120);
  sheet.setColumnWidth(6, 100);
}

function testScript() {
  var sheet = SpreadsheetApp.getActiveSheet();
  var serverTimestamp = new Date();
  
  sheet.appendRow([
    serverTimestamp,
    12345,
    2.456,
    0.123,
    'Good',
    'flowmeter'
  ]);
  
  console.log('Test data added to sheet');
}
