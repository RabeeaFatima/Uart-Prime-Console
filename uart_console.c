////////////
//UART 
//Rabeea Fatima
//////////
#include <stdint.h> // so I can use the integers I want
#include <stdio.h>  // so I can use sprintf
#include "sleep.h"


#define UART1_CR    *((uint32_t *) 0xE0001000)// UART1 control register
#define UART1_MR    *((uint32_t *) 0xE0001004)// UART1 Mode register
#define UART1_BAUDGEN	*((uint32_t *) 0xE0001018)
#define UART1_BAUDDIV	*((uint32_t *) 0xE0001034)

#define UART1_SR	*((uint32_t *) 0xE000102C)	//UART1 status reg
#define UART1_DATA	*((uint32_t *) 0xE0001030)	//UART1 TX/RX FIFO DATA


#define BaudGen115200 0x7c
#define BaudDiv115200 6

//check if room for bytes in tx fifo
int uart1_tx_full()
{
	return (UART1_SR & 16)!=0;			//Check SR register bit4, return 1 if fifo full
}

void uart1_put_char (char dat)
{
	 			//wait for room in FIFO
	UART1_DATA = dat;				//write to TX FIFO
}

void uart1_putstr(char *ToSend){ // Takes a pointer to a null terminated string
    if (!ToSend) return;
    while (*ToSend) {
        char c = *ToSend++;
        // send CR before LF for clean newlines
        if (c == '\n') {
            while (uart1_tx_full()) { /* wait for room */ }
            UART1_DATA = '\r';
        }
        while (uart1_tx_full()) { /* wait for room */ }
        UART1_DATA = (uint32_t)c;
    }
}


int UART1_RXEmpty (){
	uint32_t UARTData; // general variable to hold data  to/from UART
	UARTData = UART1_SR;
	if ((UARTData & 0x02) == 2){
		return 1;
	}else {
		return 0;
	}
}

char uart1_get_char ()
{
	 	//wait for room something in FIFO
	return (char)UART1_DATA; // return data from the FIFO
}

int GetInteger(){ // retrieves an integer from the serial port.

	char OneChar;
    char buf[16];
    int idx = 0;

	// build up the string from the user, then convert it to an integer. 
	// make sure you only accept ascii values that are integers
	// wait for the 0x0d to know the user has pressed enter.

    while (1) {
        // poll until a character is available, without changing uart1_get_char()
        while (UART1_RXEmpty()) { /* spin */ }

        OneChar = uart1_get_char(); // get one character WAIT FOR IT.

        // ENTER (0x0D or 0x0A) ends the number
        if (OneChar == '\r' || OneChar == '\n') {
            uart1_put_char('\r'); // echo newline nicely
            uart1_put_char('\n');
            break;
        }

        // Handle backspace/delete
        if (OneChar == 0x08 || OneChar == 0x7F) {
            if (idx > 0) {
                idx--;
                // erase on terminal
                uart1_put_char('\b');
                uart1_put_char(' ');
                uart1_put_char('\b');
            }
            continue;
        }

        // Accept digits only
        if (OneChar >= '0' && OneChar <= '9') {
            if (idx < (int)(sizeof(buf) - 1)) {
                buf[idx++] = OneChar;
                uart1_put_char(OneChar); // echo what user typed
            }
            // ignore extra digits beyond buffer
        } else {
            // ignore non-digit characters
        }
    }

    if (idx == 0) {
        return -1; // empty input
    }

    // null-terminate and convert to integer
    buf[idx] = '\0';
    int value = 0;
    for (int i = 0; i < idx; i++) {
        value = value * 10 + (buf[i] - '0');
    }
    return value;
}


// Need to initialize the UART
//
//
void ResetUART1(){
// as part of reset disable the tx and rx to prevernt errant things from going out.     
	UART1_CR = 0x17;  // assert the reset of the UART / disable transmit and recv
	while ((UART1_CR & 0x2)  == 2){} // wait till xmit reset clears
	while ((UART1_CR & 0x1)  == 1){} // wait till rcv reset cleared by the UART
	}

void Configure_UART1 (){

	uint32_t UARTData; // general variable to hold data  to/from UART
	//configure the mode register
	//normal mode bits [9:8] = 2b00
	// mode = auto echo bits [9:8] =01
	//one stop bit, bits [7:6] = 2b00
	//8 data bits, bits [2:1] = 2b0x
	//use reference clock, bit0 = 0
	//disable parity, bits[5:3] = 3b1xx
	//6b100000 == 0x20 // no echoe
	// 9b100000 == 0x120 // auto echo

	UART1_MR = 0x20;
	// enable the transmit and recieve  OR 0x14 to CR register
	UARTData = UART1_CR; //get the current setting and OR in the
	UARTData |= 0x14;    // Enable TX and RX
	UART1_CR = UARTData;

}

void SetBaudrate1(){
	// set to 115200 baudrate
	UART1_BAUDGEN = BaudGen115200;
	UART1_BAUDDIV = BaudDiv115200;

}

void initUart1() {
	//Set to 115200 8,n,1
	ResetUART1(); // reset the uart to start and disable the TX & RX
	SetBaudrate1(); // set baudrate to 115200
	Configure_UART1(); // configure to 8,n,1
}



// ---- ADDED: Prime helper (no changes to existing code above) --------------
static int isPrime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if ((n & 1) == 0) return 0;
    for (int d = 3; (long)d * d <= n; d += 2) {
        if (n % d == 0) return 0;
    }
    return 1;
}

int main (){
	char SendString[90];

	initUart1(); // set uart1 to 115200 8 n 1

for (;;){

// ask user for a number 
// get the number
// check to see if it is prime
// if it is not prime then subtract 1 from the number check for prime,
// repeat until you find the first prime number less than the user entered number.
// print out to the prime number closest to teh number entered on teh terminal.

    uart1_putstr("Enter a positive integer (>= 2), then press ENTER: ");

    int n = GetInteger();
    while (n < 2) {
        uart1_putstr("Invalid input. Please enter an integer >= 2: ");
        n = GetInteger();
    }

    int candidate = n;
    while (candidate >= 2 && !isPrime(candidate)) {
        candidate--;
    }

    if (candidate >= 2) {
        // use your existing SendString buffer and sprintf (as requested)
        sprintf(SendString, "Largest prime <= %d is: %d\n", n, candidate);
        uart1_putstr(SendString);
    } else {
        uart1_putstr("No prime found.\n");
    }

    uart1_putstr("\n");
}

	return (1);
}

