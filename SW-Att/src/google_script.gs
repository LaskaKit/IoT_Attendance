function doGet(e) { 
  Logger.log( JSON.stringify(e) );  		// view parameters
  var result = 'Ok';                    // assume success
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1ItukBqCmwK7BvR5UqjVkHKqf73Gq5wfZPUnz46H_7kw'; 		// Spreadsheet ID
		
    var rowData = [];
    var date = new Date();    						// Timestamp
    var newRow = 0;
    var name;
    var enter;
    var exit;
    
    var oldDate;
    var newDate;
    
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'name': 							//Parameter
          name = value;
          break;
        default:
          result = "unsupported parameter";
      }
    }
    var sheet = SpreadsheetApp.openById(sheet_id).getSheetByName(name);		//getSheetByName('naam') voor ander blad 
 
    newRow = sheet.getLastRow();
    oldDate = sheet.getRange(newRow, 1).getValue();
    oldDate = Utilities.formatDate(oldDate, Session.getScriptTimeZone(), "d.M.yyyy");
    exit = sheet.getRange(newRow, 3).getValue();
    newDate = Utilities.formatDate(date, Session.getScriptTimeZone(), "d.M.yyyy");
    
    if (exit == '' &&  oldDate == newDate) {
      rowData[0] = newDate;
      enter = sheet.getRange(newRow, 2).getValue();
      rowData[1] = enter;      
      rowData[2] = date.toLocaleTimeString();   
    } else {
      rowData[0] = newDate;
      newRow = newRow + 1;
      rowData[1] = date.toLocaleTimeString();  
    }

    Logger.log(JSON.stringify(rowData));
    // Write new row below
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }
  // Return result of operation
  return ContentService.createTextOutput(result);
}
/**
* Remove leading and trailing single or double quotes
*/
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}