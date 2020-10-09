//----------------------------------------//
//  bmp180.ino
//
//  created 02 09 2018
//  by Luiz H. Cassettari
//----------------------------------------//


#include <Wire.h>
#include <Adafruit_BMP085.h>

boolean bmp180_on = false;

float _bmpTemp = 0;
float _bmpPres = 0;

boolean bmp180_setup()
{
  Adafruit_BMP085 bmp;
  if (bmp.begin())
  {
    _bmpTemp = 0;
    _bmpPres = 0;

    for (int i = 0; i < 10; i++) {
      delay(10);
      _bmpTemp += bmp.readTemperature();
      _bmpPres += bmp.readPressure() / 100.0;
    }

    _bmpTemp /= 10.0;
    _bmpPres /= 10.0;

    return true;
  }
  return false;
}


static boolean mode = false;

float bmpTemp() {
  return _bmpTemp;
}

float bmpPres() {
  return _bmpPres;
}
