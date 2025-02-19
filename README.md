# Projeto Joystick com LEDs e Display OLED

Este projeto utiliza um joystick analógico, LEDs RGB e um display OLED SSD1306 para criar uma interface interativa. Ele usa o microcontrolador Raspberry Pi Pico e a biblioteca `pico-sdk` para controlar os LEDs via PWM, ler os valores do joystick e exibir informações no display OLED.

## Funcionalidades

- **Joystick Analógico**: Controle de LEDs RGB usando os valores do joystick nos eixos X e Y.
- **Display OLED**: Exibição de gráficos e formas (como retângulos e linhas) baseados nas leituras do joystick.
- **LEDs RGB**: Controle da intensidade dos LEDs RGB com base na posição do joystick.
- **Botões**: Controle de LED verde e troca de formas desenhadas no display com botões físicos conectados ao Raspberry Pi Pico.

## Componentes Utilizados

- **Raspberry Pi Pico**: Microcontrolador para controle do sistema.
- **Joystick Analógico**: Para controlar os LEDs RGB e desenhar no display.
- **Display OLED SSD1306**: Display de 128x64 pixels usado para exibir formas e gráficos.
- **LEDs RGB (Red, Green, Blue)**: Para feedback visual do controle do joystick.
- **Botões**: Para alternar entre diferentes modos de exibição no display e ativar/desativar PWM.

## Conexões de Hardware

- **Joystick**:
  - X-Axis: GPIO 26 (ADC 0)
  - Y-Axis: GPIO 27 (ADC 1)
  - Botão: GPIO 22

- **LEDs**:
  - Vermelho: GPIO 13 (PWM)
  - Verde: GPIO 11 (PWM)
  - Azul: GPIO 12 (PWM)

- **Botões**:
  - Botão 22: GPIO 22
  - Botão A: GPIO 5
  - Botão B: GPIO 6

- **Display OLED SSD1306 (via I2C)**:
  - SDA: GPIO 14
  - SCL: GPIO 15

## Como Compilar e Rodar

### Requisitos

- Raspberry Pi Pico
- Ambiente de desenvolvimento com `pico-sdk` configurado
- CMake para compilação
- Ferramentas para upload do código para o Raspberry Pi Pico

### Passos

1. **Configuração do Ambiente**:
   - Instale o SDK do Raspberry Pi Pico seguindo a documentação oficial.
   - Clone este repositório e configure o ambiente de desenvolvimento com `pico-sdk`.

2. **Compilação**:
   - Execute o seguinte comando no diretório do projeto para compilar o código:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
Carregar no Raspberry Pi Pico:

Conecte o Raspberry Pi Pico ao computador em modo de bootloader.
Copie o arquivo .uf2 gerado para o dispositivo de armazenamento do Pico.
Execução:

Após o upload do código, o Raspberry Pi Pico iniciará automaticamente e você verá as interações com os LEDs RGB e o display OLED baseadas no joystick e botões.
Estrutura do Código
Funções principais:
setup_joystick(): Configura os pinos para leitura do joystick.
setup_pwm(): Configura o controle PWM dos LEDs RGB.
setup_system(): Configuração geral do sistema, incluindo LEDs, botões e joystick.
read_joystick(): Lê os valores do joystick usando o ADC.
setup_i2c(): Configura a comunicação I2C para o display OLED.
button_callback(): Função de interrupção para gerenciar o comportamento dos botões.
Interrupções:
O código usa interrupções para detectar quando os botões são pressionados, alterando o estado dos LEDs e alterando a forma exibida no display OLED.
