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

    // Functions to activate individual colors
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

    // Function to set the color based on intensity for each channel
    void setColor(int redIntensity, int greenIntensity, int blueIntensity) {  
        lightRed = !redIntensity;
        lightGreen = !greenIntensity;
        lightBlue = !blueIntensity;
        lightState = 1;
    }

    // Function to turn off the RGB light
    void deactivate() {
        lightRed = 1;
        lightGreen = 1;
        lightBlue = 1;
        lightState = 0;
    }
};

C12832 display(D11, D13, D12, D7, D10); // Display initialization
RGBLED myRGBLED(D5, D9, D8); // RGB light initialization
Timer timer;

int firstNumber = 1;
int secondNumber = 1;
char mathOps[] = {'+', '-', '*', '/', '^'}; // Mathematical operations
int selectedOp = 0; // Selected operation index
float calculationResult = 2.00;

int selectionCursor = 0; // Cursor for selection
Ticker cursorBlink; // Ticker for cursor blinking
bool isCursorVisible = true; // Cursor visibility flag

// Global variable to track the previous cursor position
int previousCursorXPosition = 20;

const int debounceDelay = 200; // milliseconds
int lastButtonPress = 0;

bool isValidPress() {
    int currentTime = timer.read_ms(); // Assuming you have a timer object
    if (currentTime - lastButtonPress > debounceDelay) {
        lastButtonPress = currentTime;
        return true;
    }
    return false;
}

// Function to clear the previous cursor on the display
void clearPreviousCursor() {
    // Clear the previous cursor by printing a space at its position
    display.locate(previousCursorXPosition, 18);
    display.printf(" ");
}

// Function to perform the calculation based on the selected operation
void performCalculation() {
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

// Function to refresh the cursor position on the display
void refreshCursor() {
    // Calculate the new cursor X position based on the selectionCursor value
    int cursorXPosition = 20 + (selectionCursor == 1 ? 10 : (selectionCursor == 2 ? 20 : 0));
    
    if (isCursorVisible) {
        // Clear the previous cursor position before drawing a new one
        clearPreviousCursor();

        // Draw the new cursor
        display.locate(cursorXPosition, 18);
        display.printf("_");
        
        // Update the previous cursor position
        previousCursorXPosition = cursorXPosition;
    } else {
        // Clear the cursor if it should not be visible
        clearPreviousCursor();
    }
}

// Function to update the LED color based on the cursor position
void updateLEDBasedOnCursorPosition() {
    // Change LED color based on cursor position
    switch (selectionCursor) {
        case 0: myRGBLED.activateRed(1); break; // First digit - Red
        case 1: myRGBLED.activateBlue(1); break; // Operator - Blue
        case 2: myRGBLED.activateYellow(1); break; // Second digit - Yellow
        default: myRGBLED.deactivate(); break; // Default to no color
    }
}

// Function to refresh the display with updated values
void refreshDisplay() {
    performCalculation();
    display.locate(20, 10);
    display.printf("%d %c %d = %.2f ", firstNumber, mathOps[selectedOp], secondNumber, calculationResult);
    
    // Correctly call the function to update the LED based on the cursor position
    updateLEDBasedOnCursorPosition();

    // Call refreshCursor() to ensure the cursor is displayed correctly
    refreshCursor();
}

// Function to increase the value at the cursor position
void increaseValue() {
    if (!isValidPress()) return;
    if(selectionCursor == 0) {
        firstNumber = (firstNumber + 1) % 10;
    } else if(selectionCursor == 1) {
        selectedOp = (selectedOp + 1) % 5;
    } else if (selectionCursor == 2) {
        secondNumber = (secondNumber + 1) % 10;
    }
    refreshDisplay();
}

// Function to decrease the value at the cursor position
void decreaseValue() {
    if (!isValidPress()) return;
    if(selectionCursor == 0) {
        firstNumber = (firstNumber - 1 < 0) ? 9 : firstNumber - 1;
    } else if(selectionCursor == 1) {
        selectedOp = (selectedOp - 1 < 0) ? 4 : selectedOp - 1;
    } else if (selectionCursor == 2) {
        secondNumber = (secondNumber - 1 < 0) ? 9 : secondNumber - 1;
    }
    refreshDisplay();
}

// Function to toggle the cursor position
void toggleCursorPosition() {
    if (!isValidPress()) return;
    selectionCursor = (selectionCursor + 1) % 3;
    refreshDisplay();
}

// Function to toggle cursor visibility using a Ticker
void toggleCursorVisibility() {
    // Toggle the cursor visibility flag
    isCursorVisible = !isCursorVisible;

    // Refresh the cursor without disabling interrupts, as this is a quick operation
    refreshCursor();
}

int main() {
    timer.start();
    display.cls();
    myRGBLED.activateRed(1);

    InterruptIn buttonUp(A2), buttonDown(A3), buttonCentre(D4);
    buttonUp.rise(&increaseValue);
    buttonDown.rise(&decreaseValue);
    buttonCentre.rise(&toggleCursorPosition);

    cursorBlink.attach(&toggleCursorVisibility, 0.5);

    // Initialize the display and cursor once
    refreshDisplay();
    
    while (true) {}
}
