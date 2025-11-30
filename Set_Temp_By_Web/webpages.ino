void handleRoot() {
  // Build HTML page
  String html = "<!DOCTYPE html><html><head>";
  html += "<title>TEMPERATURE CONTROLLER</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin-top:20px;}";
  html += ".card{background:#f2f2f2;border-radius:10px;padding:20px;";
  html += "display:inline-block;margin:10px;width:300px;}";
  html += "input{width:80%;padding:12px;margin:8px 0;border:1px solid #ccc;}";
  html += "button{background:#1723ad;color:white;padding:14px 20px;";
  html += "border:none;cursor:pointer;width:85%;}";
  html += "</style></head><body>";
  html += "<h1>TEMPERATURE CONTROLLER</h1>";
  
  // Current values card
  html += "<div class='card'>";
  html += "<h2>CURRENT VALUES</h2>";

  html += "<p>TEMP: <strong id='tempVal'>--</strong></p>"; 
  html += "<p>TEMP RELAY STATUS: <strong id='temprelay'>--</strong></p>"; 
  html += "<p>IS TEMP IN FORWARD STATUS: <strong id='forwardtempVal'>--</strong></p>"; 

  html += "<br>";

  html += "<p> WATER: <strong id='waterval'>--</strong></p>";
  html += "<p> WATER RELAY STATUS: <strong id='waterrelay'>--</strong></p>";
  html += "<p> IS WATER IN FORWARD STATUS: <strong id='forwardwaterVal'>--</strong></p>";

  html += "<br>";
  html += "<p> SLAVE ID IS: <strong id='IDVal'>--</strong></p>";

  html += "</div>";
  
  // Setpoints card
  html += "<div class='card'>";
  html += "<h2>SETPOINTS</h2>";

  // Write the value of the value of the temperature update
  html += "<form action='/updatetemp' method='POST'>";
  html += "<label> MINIMUM TEMP:</label>";
  html += "<br>";
  html += "<input type='number' name='a' value='" + String(SET_POINT_LO) + "' required>";
  html += "<br>";
  html += "<label> MAXIMUM TEMP:</label>";
  html += "<br>";
  html += "<input type='number' name='b' value='" + String(SET_POINT_HI) + "' required>";
  html += "<br>";
  html += "<label> IS RELAY IN FORWARD MODE:</label>";
  html += "<input type='checkbox' name='c' value='on' checked>";
  html += "<br>";
  html += "<button type='submit'>SAVE TEMPERATURE SETTINGS</button>";
  html += "<br>";
  html += "</form>";

  //write the value of the voltage divider/watertank update
  html += "<form action='/updatewater' method='POST'>";
  html += "<br>";
  html += "<label> SET IGNORED WATER % VALUE:</label>";
  html += "<br>";
  html += "<input type='number' name='percentage' value='" + String(percentageignored) + "' required>";
  html += "<br>";
  html += "<label> SET MINIMUM WATER % VALUE:</label>";
  html += "<br>";
  html += "<input type='number' name='min' value='" + String(minwaterpercentvalue) + "' required>";
  html += "<br>";
  html += "<label> SET MAXIMUM WATER % VALUE:</label>";
  html += "<br>";
  html += "<input type='number' name='max' value='" + String(maxwaterpercentvalue) + "' required>";
  html += "<br>";
  html += "<label> IS WATER RELAY IN FORWARD MODE:</label>";
  html += "<input type='checkbox' name='waterM' value='on' checked>";
  html += "<br>";
  html += "<button type='submit'>SAVE WATER SETTINGS</button>";
  html += "<br>";
  html += "</form>";

  html += "<form action='/updateid' method='POST'>";
  html += "<br>";
  html += "<label> SLAVE ID: </label>";
  html += "<br>";
  html += "<input type='number' name='d' value='" + String(SLAVE_ID) + "' required>";
  html += "<br>";
  html += "<button type='submit'>SAVE ID SETTINGS</button>";
  html += "</form>";


  html += "</div></body>";

  // Updates the webpage while open without refreshing, using the handleStatus function
  html += "<script>";

  html += "function fetchStatus() {";
  html += "fetch('/status')";
  html += ".then(response => response.json())";
  html += ".then(data => {";
  html += "document.getElementById('tempVal').textContent = data.temp;";
  html += "document.getElementById('temprelay').textContent = data.temprelay;";
  html += "document.getElementById('forwardtempVal').textContent = data.tempforward;";
  html += "document.getElementById('IDVal').textContent = data.ID;";
  
  html += "document.getElementById('waterval').textContent = data.water;";
  html += "document.getElementById('waterrelay').textContent = data.waterrelay;";
  html += "document.getElementById('forwardwaterVal').textContent = data.waterforward;";

  html += "})";
  html += ".catch(err => console.log(\"Error:\", err));";
  html += "}";
  html += "setInterval(fetchStatus, 1000);";//refresh interval

  html += "</script>";
  html += "</html>";
  
  server.send(200, "text/html", html);
}
//===================================================================
void handleUpdateTemp() {
  // Process form submission
  if (server.hasArg("a") && server.hasArg("b")) 
  {
    int newA = server.arg("a").toInt();
    int newB = server.arg("b").toInt();
    String ccheck = server.arg("c");
    bool newC;
    if(ccheck == "on")
      newC = true;
    else
      newC = false;
    
    Serial.print("newA : ");
    Serial.println(newA);
    Serial.print("newB : ");
    Serial.println(newB);
    Serial.print("newC : ");
    Serial.println(newC);
    // Validate input
    if (newA >= 0 && newB > newA) {
      SET_POINT_LO = newA;
      SET_POINT_HI = newB;
      isForward = newC;
      saveSettings();
    }
    server.sendHeader("Location", "/");
    server.send(303);
  }
}

void handleUpdateWater(){
  if(server.hasArg("percentage") && server.hasArg("max") && server.hasArg("min"))
  {
    int newPerc = server.arg("percentage").toInt();
    int newMax = server.arg("max").toInt();
    int newMin = server.arg("min").toInt();
    bool newM;
    String Mstring = server.arg("waterM");
    if(Mstring == "on")
      newM = true;
    else
      newM = false;

    Serial.print("New Percentage: ");
    Serial.println(newPerc);
    Serial.print("New Maximum: ");
    Serial.println(newMax);
    Serial.print("New Minimum:");
    Serial.println(newMin);
    Serial.print("New Mode: ");
    Serial.println(newM);

    // Validate input

    if(newMax > newMin && newMax>0 && newMax<=100 && newMin >=0 && newMin<100 &&newPerc <=100 && newPerc >=0)
    {
      percentageignored = newPerc;
      maxwaterpercentvalue = newMax;
      minwaterpercentvalue = newMin;
      iswaterforwardmode = newM;
      saveSettings();
    }

  }

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleID(){
  if(server.hasArg("d"))
  {
    int newID = server.arg("d").toInt();
    Serial.println("newID : ");
    Serial.println(newID);

    if(newID >0 && newID<248)
    {
      if(SLAVE_ID != newID)
        {
          SLAVE_ID = newID;
          MB.slave(SLAVE_ID);
        }
      
      Serial.print("NEW ID: ");
      Serial.println(SLAVE_ID);
      saveSlaveID();
    }
    else
    {
      SLAVE_ID = 1;
      Serial.print("NEW ID: ");
      Serial.println(SLAVE_ID);
      saveSlaveID();
    }
    server.sendHeader("Location", "/");
    server.send(303);
  }
}

void handleStatus() {
  String json = "{";
  json += "\"temp\":" + String(temp) + ",";
  json += "\"temprelay\":\"" + String(relayStateTemp ? "ON" : "OFF") + "\",";
  json += "\"tempforward\":\"" + String(isForward ? "YES" : "NO") + "\",";
  json += "\"ID\":\"" + String(SLAVE_ID) + "\",";

  json += "\"water\":" + String(currentwaterlevel) + ",";
  json += "\"waterrelay\":\"" + String(relayStateWater ? "ON" : "OFF") + "\",";
  json += "\"waterforward\":\"" + String(iswaterforwardmode ? "YES" : "NO") + "\"";

  json += "}";
  server.send(200, "application/json", json);
}