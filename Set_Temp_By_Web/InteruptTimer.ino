void timer1interupt()
{
  if(!blinkingled)
  {
    digitalWrite(ledPin, false);
    running = false;
  }
  else
  {
    running = true;
    ledstate = !ledstate;
    digitalWrite(ledPin, ledstate);
  }
}