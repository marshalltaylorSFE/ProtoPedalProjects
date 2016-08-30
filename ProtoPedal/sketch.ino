#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=153,181
AudioFilterBiquad        biquad2;        //xy=357,128
AudioFilterBiquad        biquad3;        //xy=357,163
AudioFilterBiquad        biquad1;        //xy=358,93
AudioMixer4              mixer1;         //xy=536,143
AudioOutputI2S           i2s2;           //xy=715,162
AudioConnection          patchCord1(i2s1, 0, biquad1, 0);
AudioConnection          patchCord2(i2s1, 0, biquad2, 0);
AudioConnection          patchCord3(i2s1, 0, biquad3, 0);
AudioConnection          patchCord4(biquad2, 0, mixer1, 1);
AudioConnection          patchCord5(biquad3, 0, mixer1, 2);
AudioConnection          patchCord6(biquad1, 0, mixer1, 0);
AudioConnection          patchCord7(mixer1, 0, i2s2, 0);
// GUItool: end automatically generated code
