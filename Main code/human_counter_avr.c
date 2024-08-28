#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL  // Define CPU frequency for delay
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#define IR_SENSOR_IN_PIN PD2
#define IR_SENSOR_OUT_PIN PD3
#define LED_PIN PB0

volatile int totalSeats = 0;
volatile int availableSeats = 0;

void uart_init(unsigned int ubrr) {
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);   // Enable receiver and transmitter
    UCSR0C = (1<<USBS0) | (3<<UCSZ00);  // Set frame format: 8 data bits, 2 stop bits
}

unsigned char uart_receive(void) {
    while (!(UCSR0A & (1<<RXC0)));  // Wait for data to be received
    return UDR0;  // Get and return received data from buffer
}

void uart_transmit(unsigned char data) {
    while (!(UCSR0A & (1<<UDRE0)));  // Wait for empty transmit buffer
    UDR0 = data;  // Put data into buffer, sends the data
}

void lcd_init(void) {
    // Assuming a 4-bit LCD setup connected to PORTB and PORTD
    // LCD initialization code goes here
}

void lcd_clear(void) {
    // LCD clear function
}

void lcd_print(char *str) {
    // Function to print a string to the LCD
}

void update_display(void) {
    lcd_clear();
    lcd_print("Seats Left: ");
    char buffer[16];
    itoa(availableSeats, buffer, 10);  // Convert integer to string
    lcd_print(buffer);
}

void setup(void) {
    // Set IR sensors as input
    DDRD &= ~(1<<IR_SENSOR_IN_PIN);
    DDRD &= ~(1<<IR_SENSOR_OUT_PIN);
    
    // Set LED pin as output
    DDRB |= (1<<LED_PIN);
    
    // Enable external interrupts on IR sensor pins
    EICRA |= (1<<ISC01) | (1<<ISC00);  // Rising edge on INT0 (IR_SENSOR_IN_PIN)
    EICRA |= (1<<ISC11) | (1<<ISC10);  // Rising edge on INT1 (IR_SENSOR_OUT_PIN)
    EIMSK |= (1<<INT0) | (1<<INT1);    // Enable INT0 and INT1
    
    uart_init(MYUBRR);  // Initialize UART for Bluetooth communication
    lcd_init();         // Initialize LCD
    
    sei();  // Enable global interrupts
}

ISR(INT0_vect) {
    _delay_ms(100);  // Debounce
    if (!(PIND & (1<<IR_SENSOR_OUT_PIN))) {
        if (availableSeats > 0) {
            availableSeats--;
            update_display();
            PORTB |= (1<<LED_PIN);  // Turn on LED
            _delay_ms(500);
            PORTB &= ~(1<<LED_PIN); // Turn off LED
        }
    }
}

ISR(INT1_vect) {
    _delay_ms(100);  // Debounce
    if (!(PIND & (1<<IR_SENSOR_IN_PIN))) {
        if (availableSeats < totalSeats) {
            availableSeats++;
            update_display();
            PORTB |= (1<<LED_PIN);  // Turn on LED
            _delay_ms(500);
            PORTB &= ~(1<<LED_PIN); // Turn off LED
        }
    }
}

int main(void) {
    setup();
    
    while (1) {
        // Check for Bluetooth input to set total seats
        if (UCSR0A & (1<<RXC0)) {
            totalSeats = uart_receive() - '0';  // Convert char to int
            availableSeats = totalSeats;
            update_display();
        }
    }
}
