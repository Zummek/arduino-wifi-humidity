var SS = SpreadsheetApp.openById('your-sheet-id');     // Enter Your Sheet ID Got From Sheet URL Link

function doPost(data) {
  var parsedData;
  var response = "";
  var currentDate = new Date();
  
  try { 
    parsedData = JSON.parse(data.postData.contents);
  } 
  catch(e){
    return ContentService.createTextOutput("Error in parsing request body: " + e.message);
  }
   
  if (parsedData === undefined)
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  
  switch (parsedData.command) {
    case "appendRow":
      try {
        var decimalData = parsedData.values.toString().replaceAll('.', ',');
        var dataArr = decimalData.split(";");
        dataArr.unshift(currentDate);

        var sheet = SS.getSheetByName(getSheetNameByDate(currentDate));
        if(!sheet)
          sheet = createDefaultSheet();
        
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

function getSheetNameByDate(date) {
  var month = date.getMonth() + 1;
  var year = date.getFullYear();
  return month.toString() + '.' + year.toString();
}

function createDefaultSheet() {
  var sheetName = getSheetNameByDate(new Date());
  var sheet = SS.getSheetByName(sheetName);
  if(!sheet)
    sheet = SS.insertSheet(sheetName, 0);
    
  sheet.appendRow(["Date", "Temperature", "Humidity"]);

 /**
  * add chart
  * 
  * GOOGLE BUG: https://issuetracker.google.com/issues/205506220
  * "Setting Series and vAxes don't work while creating, modifying, coping chart"
  *  
  * Not all chart's options are working. So it's simple chart
  */
  var range = sheet.getRange('A1:C1');
  var chart = sheet.newChart()
    .asLineChart()
    .addRange(range)
    .setChartType(Charts.ChartType.LINE)
    .setPosition(1, 4, 0, 0)
    .setNumHeaders(1)
    .setOption('title', 'Temperature i humidity | ' + sheetName)
    .setOption('legend', {
      position: 'top'
    })
    .setOption('width', 1000)
    .setOption("useFirstColumnAsDomain", true)
    .setOption('curveType', 'function')
    .setOption('series', {         // series NOT WORKING
      0: {
        type: 'line',
        color: 'red',
        targetAxisIndex: 0 
      },
      1: {
        type: 'line',
        color: 'blue',
        targetAxisIndex: 1
      }
    })
    .setOption("vAxes", [         // vAxes NOT WORKING
      {title: "Temperature [°C]"},
      {title: "Humidity [%]"},
    ])
    .setOption('hAxis', {
      title: 'Date',
      gridlines: {
        count: 10
      },
      minorGridlines: {
        count: 5
      },
    }).build();
    
  sheet.insertChart(chart);

  return sheet;
}