// Sample code for ECE 198

// Written by Bernie Roehl, August 2021

// This file contains sample code for a number of different applications.
// Each one is surrounded by an #ifdef ... #endif block inside of main().

// To enable a particular application, you should remove the comment (//) in
// front of exactly ONE of the following lines:

#define BUTTON_BLINK
//#define LIGHT_SCHEDULER
//#define FAN_SCHEDULER
//#define TIME_RAND
//#define KEYPAD
//#define SEVEN_SEGMENT
//#define KEYPAD_SEVEN_SEGMENT
//#define COLOR_LED
//#define ROTARY_ENCODER
//#define PWM
//#define ANALOG

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"

int main(void)
{
    HAL_Init(); // initialize the Hardware Abstraction Layer

    // Peripherals (including GPIOs) are disabled by default to save power, so we
    // use the Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    // initialize the pins to be input, output, alternate function, etc...

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0); // initialize the pin that the on-board LED is on

    // note: the on-board pushbutton is fine with the default values (input, and no pull-up resistor is required since there's one on the board)

    // set up for serial communication to the host computer
    // (anything we write to the serial port will appear in the console in VSCode)

    SerialSetup(9600);

    // as mentioned above, only one of the following code sections will be used
    // (depending on which of the #define statements at the top of this file has been uncommented)

#ifdef BUTTON_BLINK
    // wait for the user to push the blue button, then blink the LED

    // wait for button press (active low)
    while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
    {
    }

    while (1) // loop forever, blinking the LED
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(250);  // 250 milliseconds == 1/4 second
    }
#endif

#ifdef LIGHT_SCHEDULER
    while (true) {
        uint32_t now = HAL_GetTick();
        if (now > 5000 && now < 10000)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);
        else
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);
    }
#endif

#ifdef FAN_SCHEDULER
    InitializePin(GPIOC, GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);
    while (true) {
        uint32_t now = HAL_GetTick();
        if (now > 5000 && now < 10000)
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, true);
        else
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, false);
    }
#endif

#ifdef TIME_RAND
    // This illustrates the use of HAL_GetTick() to get the current time,
    // plus the use of random() for random number generation.
    
    // Note that you must have "#include <stdlib.h>"" at the top of your main.c
    // in order to use the srand() and random() functions.

    //while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button press
    //srand(HAL_GetTick());    // set the random seed to be the time in milliseconds that it took the user to press the button
    // if the line above is commented out, your program will get the same sequence of random numbers
    // every time you run it (which may be useful in some cases)

    while (true) // loop forever
    {
        while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button press

        // Display the time in milliseconds along with a random number.
        // We use the sprintf() function to put the formatted output into a buffer;
        // see https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm for more
        // information about this function
        char buff[100];
        sprintf(buff, "Time: %lu ms   Random = %ld\r\n", HAL_GetTick(), random());  // lu == "long unsigned", ld = "long signed"
        SerialPuts(buff); // transmit the buffer to the host computer where it gets displayed in the VSCode serial console

        while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button to be released
    }
#endif

#ifdef KEYPAD
    // Read buttons on the keypad and display them on the console.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypad layouts)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    while (true)
    {
        while (ReadKeypad() < 0);   // wait for a valid key
        SerialPutc(keypad_symbols[ReadKeypad()]);
        while (ReadKeypad() >= 0);  // wait until key is released
    }
#endif

#ifdef SEVEN_SEGMENT
    // Display the numbers 0 to 9 inclusive on the 7-segment display, pausing for a second between each one.
    // Remember that the GND connection on the display must go through a 220 ohm current-limiting resistor!
    Initialize7Segment();
    while (true)
        for (int i = 0; i < 10; ++i)
        {
            Display7Segment(i);
            HAL_Delay(1000);  // 1000 milliseconds == 1 second
        }
#endif

#ifdef KEYPAD_SEVEN_SEGMENT
    // Combines the previous two examples, displaying numbers from the keypad on the 7-segment display.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypad layouts)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    Initialize7Segment();
    while (true)
    {
        int key = ReadKeypad();
        if (key >= 0)
            Display7Segment(keypad_symbols[key]-'0');
    }
#endif

#ifdef COLOR_LED
    // Cycle through all 8 possible colors (including off and white) as the on-board button is pressed.
    // This example assumes that the color LED is connected to pins D11, D12 and D13.

    // Remember that each of those three pins must go through a 220 ohm current-limiting resistor!
    // Also remember that the longest pin on the LED should be hooked up to GND.

    InitializePin(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // initialize color LED output pins
    while (true) {
        for (int color = 0; color < 8; ++color) {
            // bottom three bits indicate which of the three LEDs should be on (eight possible combinations)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, color & 0x01);  // blue  (hex 1 == 0001 binary)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, color & 0x02);  // green (hex 2 == 0010 binary)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, color & 0x04);  // red   (hex 4 == 0100 binary)

            while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));   // wait for button press 
            while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button release
        }
    }
#endif

#ifdef ROTARY_ENCODER
    // Read values from the rotary encoder and update a count, which is displayed in the console.

    InitializePin(GPIOB, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count = 0;             // this gets incremented or decremented as we rotate the encoder

    while (true)
    {
        count += ReadEncoder(GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, &previousClk);  // update the count by -1, 0 or +1

        char buff[100];
        sprintf(buff, "%d     \r", count);
        SerialPuts(buff);

        bool sw = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);  // read the push-switch on the encoder (active low, so we invert it using !)
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, sw); // turn on LED when encoder switch is pushed in
    }
#endif

#ifdef PWM
    // use Pulse Width Modulation to fade the LED in and out
    uint16_t period = 100, prescale = 16;

    __TIM2_CLK_ENABLE();  // enable timer 2
    TIM_HandleTypeDef pwmTimerInstance;  // this variable stores an instance of the timer
    InitializePWMTimer(&pwmTimerInstance, TIM2, period, prescale);   // initialize the timer instance
    InitializePWMChannel(&pwmTimerInstance, TIM_CHANNEL_1);          // initialize one channel (can use others for motors, etc)

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2); // connect the LED to the timer output

    while (true)
    {
        // fade the LED in by slowly increasing the duty cycle
        for (uint32_t i = 0; i < period; ++i)
        {
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, i);
            HAL_Delay(5);
        }
        // fade the LED out by slowly decreasing the duty cycle
        for (uint32_t i = period; i > 0; --i)
        {
            SetPWMDutyCycle(&pwmTimerInstance, TIM_CHANNEL_1, i);
            HAL_Delay(5);
        }
    }
#endif
    return 0;
}

void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}








#ifdef ANALOG
    // read an analog input
    // https://microcontrollerslab.com/adc-stm32f4-discovery-board-with-hal-adc-driver/
    // PA4 == AN4, PB0 == AN8
    InitializePin(GPIOA, GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_NOPULL, 0);
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    ADC_HandleTypeDef adc;
    adc.Instance = ADC1;
    adc.Init.Resolution = ADC_RESOLUTION12b;
    adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    // adc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    // adc.Init.ScanConvMode = DISABLE;
	// adc.Init.ContinuousConvMode = ENABLE;
	// adc.Init.DiscontinuousConvMode = DISABLE;
    // adc.Init.NbrOfConversion = 1;
    // adc.Init.DMAContinuousRequests = DISABLE;
    // adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    HAL_ADC_Init(&adc);
    
    // ADC_ChannelConfTypeDef firstChannel;
    // firstChannel.Channel = ADC_CHANNEL_4;
    // firstChannel.Rank = 1;
    // firstChannel.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    // // firstChannel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    // HAL_ADC_ConfigChannel(&adc, &firstChannel);
    
    ADC_ChannelConfTypeDef secondChannel;
    secondChannel.Channel = ADC_CHANNEL_8;
    secondChannel.Rank = 1;
    secondChannel.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    HAL_ADC_ConfigChannel(&adc, &secondChannel);

    while (true) {
        HAL_ADC_Start(&adc);       
        if (HAL_ADC_PollForConversion(&adc, 5) == HAL_OK) {
            uint32_t value = HAL_ADC_GetValue(&adc);
            char buff[100];
            sprintf(buff, "%u   \r", (unsigned int) value);
            SerialPuts(buff);
        }
        // if (HAL_ADC_PollForConversion(&adc, 5) == HAL_OK) {
        //     uint32_t value = HAL_ADC_GetValue(&adc);
        //     char buff[100];
        //     sprintf(buff, "%u   \r", (unsigned int) value);
        //     SerialPuts(buff);
        // }
        // HAL_ADC_Stop(&adc);
        // HAL_Delay(200);
    }
#endif