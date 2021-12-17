# pseudolog
An Arduino based adapter which converts digital on/off button action to pseudo analog joystick commands.

3D model is in OpenSCAD. If you just want to print the STLs, use the latest verion (alpha order). _TOP should be flipped Y 180 degrees in your slicer.
No supports needed. The cutout circles will print fine.

Sized for an Arduino Pro Micro (Leo). Will also fit a QT PY.

Four jacks get wired to the analog pins. Sensitivity pot to another analog pin. Common power goes to jacks. Common power and Ground go to slide pot. Wiper on slide pot goes to analog pin.

While I'm working on proper schematics, here's the wiring:

Up > A0 (gray wire)
Down > A1 (purple wire)
Left > A2 (orange wire)
Right > A3 (brown wire)

(colors don't matter, but that's what in my photos)

Slide Pot is tied to A10. Slide pot also needs VCC and GND. VCC to pin 1 on pot, GND to pin 3 on pot, A10/signal/wiper to pin 2 on pot.

NeoPixel (optional) tied to pin 6. (Code not yet written.) Also needs VCC and GND.
