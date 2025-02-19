#include <stdio.h>
#include "pico/stdlib.h"        // Biblioteca padrão para microcontrolador Raspberry Pi Pico
#include "hardware/adc.h"       // Biblioteca para controle do ADC (Conversor Analógico para Digital)
#include "hardware/pwm.h"       // Biblioteca para controle do PWM (modulação por largura de pulso)
#include "inc/ssd1306.h"        // Biblioteca para controle de display OLED SSD1306
#include "inc/font.h"           // Biblioteca para fontes para o display OLED
#include "hardware/i2c.h"       // Biblioteca para controle de I2C (protocolo de comunicação)

#define I2C_PORT i2c1            // Define o barramento I2C como i2c1
#define I2C_SDA_PIN 14           // Define o pino 14 como SDA (dados) do I2C
#define I2C_SCL_PIN 15           // Define o pino 15 como SCL (clock) do I2C
#define DISPLAY_ADDRESS 0x3C     // Endereço do display SSD1306 no barramento I2C

// Variáveis para controle de tempo e estados
static volatile uint32_t last_time = 0;
static volatile uint32_t last_time1 = 0;
static volatile uint32_t last_time2 = 0;
static volatile uint8_t counter = 0;

// Variáveis para controle dos LEDs e PWM
bool green_led_on = false;   // Estado do LED verde
bool pwm_enabled = true;      // Habilitação do PWM

// Definições dos pinos e constantes para o joystick, botões e LEDs
const int joystick_x_pin = 26;   // Pino para o eixo X do joystick
const int joystick_y_pin = 27;   // Pino para o eixo Y do joystick
const int adc_channel_x = 0;     // Canal ADC para o eixo X
const int adc_channel_y = 1;     // Canal ADC para o eixo Y
const int joystick_button_pin = 22; // Pino para o botão do joystick
const int red_led_pin = 13;       // Pino para o LED vermelho
const int green_led_pin = 11;     // Pino para o LED verde
const int blue_led_pin = 12;      // Pino para o LED azul
const int button_22_pin = 22;     // Pino do botão 22
const int button_a_pin = 5;       // Pino do botão A
const int button_b_pin = 6;       // Pino do botão B
const float pwm_divider = 40.0;   // Divisor do PWM
const uint16_t pwm_period = 500;  // Período do PWM


// Variáveis para controle do PWM dos LEDs
uint pwm_slice_red_led;
uint pwm_slice_green_led;
uint pwm_slice_blue_led;

// Estrutura para o display OLED
ssd1306_t display;

// Declaração das funções
void setup_joystick();
void setup_pwm(uint led_pin, uint *pwm_slice, uint16_t level);
void setup_system();
void read_joystick(uint16_t *x_value, uint16_t *y_value);
void setup_i2c();
void button_callback(uint gpio, uint32_t events);

int main()
{
    setup_system(); // Configuração inicial do sistema
    setup_i2c();    // Configuração do barramento I2C

    // Variáveis para armazenar leituras do joystick
    uint16_t joystick_x_value, joystick_y_value;

    // Configuração das interrupções para os botões
    gpio_set_irq_enabled_with_callback(button_a_pin, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(button_b_pin, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(button_22_pin, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    while (1)
    {
        // Lê os valores do joystick
        read_joystick(&joystick_x_value, &joystick_y_value);

        // Converte os valores do joystick para PWM
        int pwm_value_x = abs(joystick_x_value * pwm_period / 4095);
        int pwm_value_y = abs(joystick_y_value * pwm_period / 4095);

        // Atualiza LEDs com valores PWM dependendo da posição do joystick
        if (pwm_enabled)
        {
            pwm_set_gpio_level(red_led_pin, abs(pwm_value_x - 250) > 30 || abs(pwm_value_x) < 25 ? pwm_value_x : 0);
            pwm_set_gpio_level(blue_led_pin, abs(pwm_value_y - 250) > 30 || abs(pwm_value_y) < 25 ? pwm_value_y : 0);
        }

        // Mapeando valores do ADC (0-4095) para resolução da tela (x: 0-127, y: 0-63)
        int x = abs((joystick_x_value * 119) / 4095);
        int y = abs((joystick_y_value * 54) / 4095);

        // Atualiza a tela OLED
        ssd1306_fill(&display, 0);
        ssd1306_square(&display, x, y, 8, 1);

        // Desenha diferentes molduras na tela dependendo do contador
        switch (counter)
        {
        case 1:
            ssd1306_rect(&display, 1, 1, 125, 60, 1, 0);
            break;
        case 2:
            ssd1306_rect(&display, 0, 1, 125, 4, 1, 1);
            ssd1306_rect(&display, 57, 1, 125, 4, 1, 1);
            break;
        case 3:
            ssd1306_line(&display, 3, 0, 121, 0, 1);
            ssd1306_line(&display, 3, 60, 121, 60, 1);
            break;
        case 4:
            ssd1306_line(&display, 1, 2, 1, 60, 1);
            ssd1306_line(&display, 125, 2, 125, 60, 1);
            break;
        default:
            ssd1306_rect(&display, 0, 0, 127, 64, 0, 0);
            break;
        }

        // Envia os dados atualizados para o display
        ssd1306_send_data(&display);

        sleep_ms(10); // Delay para evitar alto consumo da CPU
    }
}

void setup_joystick()
{
    adc_init();
    adc_gpio_init(joystick_x_pin);
    adc_gpio_init(joystick_y_pin);

    gpio_init(joystick_button_pin);
    gpio_set_dir(joystick_button_pin, GPIO_IN);
    gpio_pull_up(joystick_button_pin);
}

void setup_pwm(uint led_pin, uint *pwm_slice, uint16_t level)
{
    gpio_set_function(led_pin, GPIO_FUNC_PWM);
    *pwm_slice = pwm_gpio_to_slice_num(led_pin);
    pwm_set_clkdiv(*pwm_slice, pwm_divider);
    pwm_set_wrap(*pwm_slice, pwm_period);
    pwm_set_gpio_level(led_pin, level);
    pwm_set_enabled(*pwm_slice, true);
}
void setup_system()
{
    stdio_init_all();
    setup_joystick();
    uint16_t red_led_level = 0;
    uint16_t green_led_level = 0;
    uint16_t blue_led_level = 0;
    setup_pwm(red_led_pin, &pwm_slice_red_led, red_led_level);
    setup_pwm(blue_led_pin, &pwm_slice_blue_led, blue_led_level);

    gpio_init(green_led_pin);
    gpio_set_dir(green_led_pin, GPIO_OUT);
    gpio_init(button_22_pin);
    gpio_init(button_a_pin);

    gpio_set_dir(button_22_pin, GPIO_IN);
    gpio_set_dir(button_a_pin, GPIO_IN);

    gpio_pull_up(button_22_pin);
    gpio_pull_up(button_a_pin);
}

void read_joystick(uint16_t *x_value, uint16_t *y_value)
{
    adc_select_input(adc_channel_x);
    sleep_ms(2);
    *y_value = adc_read(); // Lê o valor do eixo Y

    adc_select_input(adc_channel_y);
    sleep_ms(2);
    *x_value = adc_read(); // Lê o valor do eixo X
}

void setup_i2c()
{
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    // Inicializa a estrutura do display
    ssd1306_init(&display, 128, 64, false, DISPLAY_ADDRESS, I2C_PORT); // Inicializa o display
    ssd1306_config(&display);                                          // Configura o display
    ssd1306_fill(&display, false);
    ssd1306_send_data(&display);
}

void button_callback(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (gpio == button_a_pin && current_time - last_time > 200000)
    {
        last_time = current_time;
        pwm_enabled = !pwm_enabled;
    }

    if (gpio == button_22_pin && current_time - last_time2 > 200000)
    {
        last_time2 = current_time;
        green_led_on = !green_led_on;
        gpio_put(green_led_pin, green_led_on);
        counter++;
        if (counter > 4)
        {
            counter = 0;
        }
    }
}
