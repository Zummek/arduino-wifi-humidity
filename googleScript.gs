var SS = SpreadsheetApp.openById('id');     // Enter Your Sheet ID Got From Sheet URL Link
var sheet = SS.getSheetByName('data');      // Enter your sheet name here
var response = "";

function doPost(data) {
  var parsedData;
  
  try { 
    parsedData = JSON.parse(data.postData.contents);
  } 
  catch(e){
    return ContentService.createTextOutput("Error in parsing request body: " + e.message);
  }
   
  if (parsedData === undefined){
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
  
  switch (parsedData.command) {
    case "appendRow":
      try {
        var decimalData = parsedData.values.toString().replaceAll('.', ',');
        var dataArr = decimalData.split(";");
        dataArr.unshift(new Date());
        
        sheet.appendRow(dataArr);
        
        response = "Success";
      } catch(e) {
        response = "Error\n" + JSON.stringify(parsedData) + "\n" + e;
      }
        SpreadsheetApp.flush();
      break; 
  }
  return ContentService.createTextOutput(response);
}