#include "mcp4922.h"
#include <hardware/spi.h>
#include <hardware/pwm.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h> // For memset

#define CLk_FREQ 100000
#define SCLK 2
#define MOSI 3
#define CS_C 5
#define CS_B 6
#define CS_A 7
#define OE 8
#define PWM 16
#define CMD_GET_DEVICE 0x01
#define CMD_SET_LASER 0x02
#define CMD_SET_MOTOR 0x03

#define START_FLAG 0x7E
#define END_FLAG 0x7F
#define DEVICE_ID 0x01
#define RESPONSE_ERROR 0xFF
#define BUFFER_SIZE 64
#define QUEUE_SIZE 10 // Number of messages in FIFO

// FIFO Queue for messages
typedef struct
{
    uint8_t data[BUFFER_SIZE];
    int length;
} Message;

Message fifo_queue[QUEUE_SIZE];
int fifo_head = 0;
int fifo_tail = 0;

uint8_t serial_buffer[BUFFER_SIZE];
int buffer_index = 0;

void checkAndExtractMessages();
void processSerialCommand();
uint8_t calculateCRC(uint8_t *args, int argCount);
void handleGetDevice();
void handleSetLaser(uint8_t channel_id, uint16_t value);
void handleSetMotor(uint16_t value);

MCP4922 dac1(spi0, CLk_FREQ, SCLK, MOSI, CS_A);
MCP4922 dac2(spi0, CLk_FREQ, SCLK, MOSI, CS_B);
MCP4922 dac3(spi0, CLk_FREQ, SCLK, MOSI, CS_C);

// FIFO Helper Functions
bool isFIFOFull() { return ((fifo_head + 1) % QUEUE_SIZE) == fifo_tail; }
bool isFIFOEmpty() { return fifo_head == fifo_tail; }

void enqueueMessage(uint8_t *data, int length)
{
    if (!isFIFOFull())
    {
        memcpy(fifo_queue[fifo_head].data, data, length);
        fifo_queue[fifo_head].length = length;
        fifo_head = (fifo_head + 1) % QUEUE_SIZE;
    }
}

bool dequeueMessage(Message *msg)
{
    if (!isFIFOEmpty())
    {
        *msg = fifo_queue[fifo_tail];
        fifo_tail = (fifo_tail + 1) % QUEUE_SIZE;
        return true;
    }
    return false;
}

int main(void)
{
    stdio_init_all();

    // Set stdin to non-blocking mode
    setvbuf(stdin, NULL, _IONBF, 0);

    printf("Pico USB Serial Ready\n");

    gpio_init(OE);
    gpio_set_dir(OE, GPIO_OUT);
    gpio_pull_up(OE);
    gpio_put(OE, 1);

    // uint16_t test = 0xFFF;

    // dac1.analogWrite(MCP4922::channel_B, test);
    // dac2.analogWrite(MCP4922::channel_B, test);
    // dac3.analogWrite(MCP4922::channel_B, test);

    while (true)
    {
        int ch = getchar_timeout_us(1); // Poll every 1us
        if (ch != -1)
        {
            serial_buffer[buffer_index++] = (uint8_t)ch;

            if (buffer_index >= BUFFER_SIZE)
            {
                buffer_index = 0; // Prevent overflow
            }

            // Extract messages from buffer and store them in FIFO
            checkAndExtractMessages();
        }

        // Process the next available command in FIFO
        if (!isFIFOEmpty())
        {
            processSerialCommand();
        }
    }
}

void checkAndExtractMessages()
{
    int startIdx = -1;
    int endIdx = -1;

    // Scan the buffer to find the first START_FLAG and END_FLAG
    for (int i = 0; i < buffer_index; i++)
    {
        if (serial_buffer[i] == START_FLAG && startIdx == -1)
        {
            startIdx = i; // Mark the start of a valid message
        }
        if (serial_buffer[i] == END_FLAG && startIdx != -1)
        {
            endIdx = i; // Found a complete message
            int messageLength = endIdx - startIdx + 1;

            // Store the valid message in FIFO
            enqueueMessage(&serial_buffer[startIdx], messageLength);

            // Remove processed message from buffer
            memmove(serial_buffer, &serial_buffer[endIdx + 1], BUFFER_SIZE - (endIdx + 1));
            buffer_index -= (endIdx + 1);

            // Reset start/end markers and continue scanning
            startIdx = -1;
            endIdx = -1;
            i = -1; // Restart loop from beginning
        }
    }

    // If buffer is full but no complete message, reset to prevent overflow
    if (buffer_index >= BUFFER_SIZE)
    {
        buffer_index = 0;
        memset(serial_buffer, 0, sizeof(serial_buffer));
    }
}

void processSerialCommand()
{
    Message msg;
    if (!dequeueMessage(&msg))
        return;

    uint8_t command, length, receivedCRC, calculatedCRC;
    uint8_t channel_id, upper, lower;
    uint16_t value;

    if (msg.length < 4)
        return; // Ignore incomplete packets

    command = msg.data[1];
    length = msg.data[2];
    receivedCRC = msg.data[length];

    // Validate CRC
    calculatedCRC = calculateCRC(&msg.data[1], length - 1);
    if (calculatedCRC != receivedCRC)
    {
        putchar(RESPONSE_ERROR);
        return;
    }

    switch (command)
    {
    case CMD_GET_DEVICE:
        handleGetDevice();
        break;
    case CMD_SET_LASER:
        channel_id = msg.data[3];
        upper = msg.data[4];
        lower = msg.data[5];
        value = (upper << 8) + lower;
        handleSetLaser(channel_id, value);
        break;
    case CMD_SET_MOTOR:
        upper = msg.data[3];
        lower = msg.data[4];
        value = (uint16_t)(upper << 8) + lower;
        handleSetMotor(value);
        break;
    default:
        putchar(RESPONSE_ERROR);
        break;
    }
}

uint8_t calculateCRC(uint8_t *args, int argCount)
{
    uint8_t crc = 0;
    for (int i = 0; i < argCount; i++)
    {
        crc ^= args[i];
    }
    return crc;
}

void handleGetDevice()
{
    uint8_t response[] = {CMD_GET_DEVICE, 0x01, DEVICE_ID};
    uint8_t crc = calculateCRC(response, 3);

    putchar(0x7E);
    for (int i = 0; i < 3; i++)
    {
        putchar(response[i]);
    }
    putchar(crc);
    putchar(0x7F);
}

void handleSetLaser(uint8_t channel_id, uint16_t value)
{
    // Array of DAC objects
    MCP4922 *dacs[] = {&dac1, &dac1, &dac2, &dac2, &dac3, &dac3};

    // Array of corresponding channels
    MCP4922::Channel channels[] = {
        MCP4922::channel_A, MCP4922::channel_B,
        MCP4922::channel_A, MCP4922::channel_B,
        MCP4922::channel_A, MCP4922::channel_B};

    if (channel_id < 6)
    {
        dacs[channel_id]->analogWrite(channels[channel_id], value);
    }
}

void setupPWM(uint gpio, uint16_t duty_cycle)
{
    // Get the slice number for this GPIO pin
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);

    // Set GPIO function to PWM
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    // Set the PWM frequency (125 MHz / wrap)
    pwm_set_wrap(slice_num, 65535); // 16-bit resolution (0-65535)

    // Set duty cycle
    pwm_set_chan_level(slice_num, channel, duty_cycle);

    // Enable PWM
    pwm_set_enabled(slice_num, true);
}

void handleSetMotor(uint16_t value)
{
    setupPWM(PWM, value);
}
