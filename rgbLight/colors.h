#ifndef RGBCOLORS_H
#define RGBCOLORS_H

// Se asume que estos valores se definen en otro archivo (por ejemplo, en tu .ino o en otro módulo):
extern int brightnessRed;
extern int brightnessGreen;
extern int brightnessBlue;
extern int maxBrightness; // Normalmente 255

// Funciones para asignar colores

// White (Blanco): (255, 255, 255)
inline void white() {
  brightnessRed   = maxBrightness;
  brightnessGreen = maxBrightness;
  brightnessBlue  = maxBrightness;
}

// Red (Rojo): (255, 0, 0)
inline void red() {
  brightnessRed   = maxBrightness;
  brightnessGreen = 0;
  brightnessBlue  = 0;
}

// Green (Verde): (0, 255, 0)
inline void green() {
  brightnessRed   = 0;
  brightnessGreen = maxBrightness;
  brightnessBlue  = 0;
}

// Blue (Azul): (0, 0, 255)
inline void blue() {
  brightnessRed   = 0;
  brightnessGreen = 0;
  brightnessBlue  = maxBrightness;
}

// Yellow (Amarillo): (255, 255, 0)
inline void yellow() {
  brightnessRed   = maxBrightness;
  brightnessGreen = maxBrightness;
  brightnessBlue  = 0;
}

// Cyan (Cian): (0, 255, 255)
inline void cyan() {
  brightnessRed   = 0;
  brightnessGreen = maxBrightness;
  brightnessBlue  = maxBrightness;
}

// Magenta (Magenta): (255, 0, 255)
inline void magenta() {
  brightnessRed   = maxBrightness;
  brightnessGreen = 0;
  brightnessBlue  = maxBrightness;
}

// Orange (Naranja): (255, 165, 0)
// Se escala el valor 165 al rango de maxBrightness
inline void orange() {
  brightnessRed   = maxBrightness;
  brightnessGreen = (165 * maxBrightness) / 255;
  brightnessBlue  = 0;
}

// Pink (Rosa): (255, 192, 203)
inline void pink() {
  brightnessRed   = maxBrightness;
  brightnessGreen = (192 * maxBrightness) / 255;
  brightnessBlue  = (203 * maxBrightness) / 255;
}

// Violet (Violeta): (238, 130, 238)
inline void violet() {
  brightnessRed   = (238 * maxBrightness) / 255;
  brightnessGreen = (130 * maxBrightness) / 255;
  brightnessBlue  = (238 * maxBrightness) / 255;
}

// Brown (Marrón): (165, 42, 42)
inline void brown() {
  brightnessRed   = (165 * maxBrightness) / 255;
  brightnessGreen = (42 * maxBrightness) / 255;
  brightnessBlue  = (42 * maxBrightness) / 255;
}

// Gray (Gris): (128, 128, 128)
inline void gray() {
  brightnessRed   = (128 * maxBrightness) / 255;
  brightnessGreen = (128 * maxBrightness) / 255;
  brightnessBlue  = (128 * maxBrightness) / 255;
}

// Olive (Verde Oliva): (128, 128, 0)
inline void olive() {
  brightnessRed   = (128 * maxBrightness) / 255;
  brightnessGreen = (128 * maxBrightness) / 255;
  brightnessBlue  = 0;
}

// Navy (Azul Marino): (0, 0, 128)
inline void navy() {
  brightnessRed   = 0;
  brightnessGreen = 0;
  brightnessBlue  = (128 * maxBrightness) / 255;
}

// Turquoise (Turquesa): (64, 224, 208)
inline void turquoise() {
  brightnessRed   = (64 * maxBrightness) / 255;
  brightnessGreen = (224 * maxBrightness) / 255;
  brightnessBlue  = (208 * maxBrightness) / 255;
}

// Gold (Oro): (255, 215, 0)
inline void gold() {
  brightnessRed   = maxBrightness;
  brightnessGreen = (215 * maxBrightness) / 255;
  brightnessBlue  = 0;
}

#endif // RGBCOLORS_H
