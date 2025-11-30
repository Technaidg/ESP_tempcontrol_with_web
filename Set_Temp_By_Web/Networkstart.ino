void networkstart()
{
  
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("LT_GEN_TEMP_V1", "03235205");
  WiFi.mode(WIFI_MODE_AP);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Web server routes
  server.on("/", handleRoot);
  server.on("/updatetemp", handleUpdateTemp);
  server.on("/updatewater", handleUpdateWater);
  server.on("/updateid", handleID);
  server.on("/status", handleStatus);
  server.begin();
}

void networkstop()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  Serial.println("Disconnected");
}