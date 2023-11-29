#include "mbed.h"
#include "C12832.h"

class RGBLED { // Class for controlling an RGB light
private:
    DigitalOut lightRed, lightGreen, lightBlue; // Digital outputs for RGB colors
    char lightState; // State of the light

public:
    RGBLED(PinName pinR, PinName pinG, PinName pinB) : lightRed(pinR), lightGreen(pinG), lightBlue(pinB) {  
        deactivate(); // Turn off the light initially                                               
    }                                                                  

    void activateRed(int intensity) {
        lightRed = !intensity;
        lightGreen = intensity;
        lightBlue = intensity;
    }

    void activateGreen(int intensity) {
        lightRed = intensity;
        lightGreen = !intensity;
        lightBlue = intensity;
    }

    void activateBlue(int intensity) {
        lightRed = intensity;
        lightGreen = intensity;
        lightBlue = !intensity;
    }

    void activateYellow(int intensity) {
        lightRed = !intensity;
        lightGreen = !intensity;
        lightBlue = intensity;
    }

    void setColor(int redIntensity, int greenIntensity, int blueIntensity) {  
        lightRed = !redIntensity;
        lightGreen = !greenIntensity;
        lightBlue = !blueIntensity;
        lightState = 1;
    }

    void deactivate() { // Turn off the RGB light
        lightRed = 1;
        lightGreen = 1;
        lightBlue = 1;
        lightState = 0;
    }
};

C12832 display(D11, D13, D12, D7, D10); // Display initialization
RGBLED myRGBLED(D5, D9, D8); // RGB light initialization

int firstNumber = 1;
int secondNumber = 1;
char mathOps[] = {'+', '-', '*', '/', '^'}; // Mathematical operations
int selectedOp = 0; // Selected operation index
float calculationResult = 2.00;

int selectionCursor = 0; // Cursor for selection
Ticker cursorBlink; // Ticker for cursor blinking
bool isCursorVisible = true; // Cursor visibility flag

void performCalculation() { // Function to perform the calculation
    char currentOp = mathOps[selectedOp];
    switch(currentOp) {
        case '+': calculationResult = firstNumber + secondNumber; break;
        case '-': calculationResult = firstNumber - secondNumber; break;
        case '*': calculationResult = firstNumber * secondNumber; break;
        case '/': calculationResult = static_cast<float>(firstNumber) / static_cast<float>(secondNumber); break;
        case '^': calculationResult = pow(static_cast<float>(firstNumber), static_cast<float>(secondNumber)); break;
        default: calculationResult = 0.00;
    }
}

void refreshDisplay() {
    performCalculation();
    display.locate(20, 10); // Location for the equation
    display.printf("%d %c %d = %.2f ", firstNumber, mathOps[selectedOp], secondNumber, calculationResult);

    if (isCursorVisible) {
        int cursorXPosition = 20; // Starting X position for the first digit

        // Adjust cursorXPosition based on the selectionCursor's value
        if (selectionCursor == 1) {
            cursorXPosition += 10; // Adjust for the operator's position
        } else if (selectionCursor == 2) {
            cursorXPosition += 20; // Adjust for the second digit's position
        }
        display.locate(cursorXPosition, 18); // Cursor position below the respective element
        display.printf("_");

        // Clear other cursor positions
        for (int i = 0; i < 3; ++i) {
            if (i != selectionCursor) {
                display.locate(20 + 10 * i, 18);
                display.printf(" ");
            }
        }
        
    }
}

void refreshCursor() {
    // Only update the cursor part of the display
    int cursorXPosition = 20; // Starting X position for the first digit

    if (selectionCursor == 1) {
        cursorXPosition += 10; // Adjust for the operator's position
    } else if (selectionCursor == 2) {
        cursorXPosition += 20; // Adjust for the second digit's position
    }
    display.locate(cursorXPosition, 18); // Cursor position below the respective element
    display.printf(isCursorVisible ? "_" : " "); // Show or hide cursor
}

void increaseValue() {
    if(selectionCursor == 0) {
        firstNumber = (firstNumber + 1) % 10;
    } else if(selectionCursor == 1) {
        selectedOp = (selectedOp + 1) % 5;
    } else if (selectionCursor == 2) {
        secondNumber = (secondNumber + 1) % 10;
    }
    refreshDisplay();
}

void decreaseValue() {
    if(selectionCursor == 0) {
        firstNumber = (firstNumber - 1 < 0) ? 9 : firstNumber - 1;
    } else if(selectionCursor == 1) {
        selectedOp = (selectedOp - 1 < 0) ? 4 : selectedOp - 1;
    } else if (selectionCursor == 2) {
        secondNumber = (secondNumber - 1 < 0) ? 9 : secondNumber - 1;
    }
    refreshDisplay();
}

void toggleCursorPosition() {
    selectionCursor = (selectionCursor + 1) % 3;
    refreshDisplay();
}

void toggleCursorVisibility() {
    isCursorVisible = !isCursorVisible;
    refreshCursor();
}

int main() {
    display.cls();
    myRGBLED.activateRed(1);

    InterruptIn buttonUp(A2), buttonDown(A3), buttonCentre(D4);
    buttonUp.rise(&increaseValue);
    buttonDown.rise(&decreaseValue);
    buttonCentre.rise(&toggleCursorPosition);

    cursorBlink.attach(&toggleCursorVisibility, 0.5);

    refreshDisplay();
    while (true) {}
}
