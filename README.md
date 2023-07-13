## 🎮 Jogo da Velha na BegleBone Black
Projeto final da disciplina de Técnicas de Programação Para Sistemas Embarcados I

### 📙 Descrição
O projeto é composto por uma matriz LEDs e botões. Os jogadores são representados por duas cores, azul e vermelho, e alternam suas jogadas. O jogo se inicia com o Player 1 (azul), os jogadores podem fazer suas jogadas pressionando os botões correspondentes às posições no tabuleiro. Cada jogada é exibida instantaneamente no tabuleiro de LEDs. Em seguida é a rodada do Player 2 (vermelho). Após cada jogada, o programa verifica automaticamente se houve um vencedor, analisando as posições marcadas no tabuleiro. Caso um jogador tente marcar uma posição já ocupada, o LED correspondente pisca para indicar que a posição está indisponível. O jogo continua até que um dos jogadores vença ou ocorra um empate, quando todas as posições do tabuleiro estão preenchidas sem um vencedor. Nesse caso, os jogadores podem optar por reiniciar o jogo pressionando um botão de reset.

### 📋 Sobre o Projeto
O jogo se trata de um projeto da disciplina Técnicas de Programação para Sistemas Embarcados I da Universidade Federal do Ceará - Campus Quixadá. Solicitado pelo professor Helder. A placa utilizada foi a BeagleBone Black, que contém o processador _AM3358x_ que integra um _ARM Cortex™-A8_. Programada na linguagem C. 

### 🛠️ Materiais
Descrição       | Quantidade  | Valor (Unid)
---------       | ----------  | -----
LEDs RGB        | 9           |R$1,00
Resistor 150Ω   | 18          |R$0,10
Transistor Bc557| 18          |R$0,90
Transformador 3v| 1           |R$5,00
Bateria 9v      | 1           |R$15,00
Botões          | 10          |R$1,00
Resistor 10kΩ   | 28          |R$0,10
Placa Perfurada | 1           |R$2,00
Caixa MDF       | 1           |R$12,00
Jumpers         |             |

### ⚙️ O Hardware
Utilizamos pull-down em todos os botões. Nos LEDs foram necessários usar transistor PNP, já que a corrente dos 18 LEDs (9 LEDs RGB) extrapolava o limite da placa. Portanto, os mesmos acendem quando o sinal está em _LOW_ (baixo).

### 💻 O Software
Configuramos interrupção para o _delay()_ e botões. Quando um botão é pressionado, uma matriz de _char_ é atualizada com o caractere 'X' ou 'O', após isso é chamada uma função onde acende os leds conforme a distribuição dos characteres citados anteriormente. Após cada rodada é verificado caso alguma linha, coluna ou diagonal está preenchida com o mesmo símbolo ASCII. Caso sim, os LEDs piscarão na suas cores correspondentes, caso haja empate, basta apertar no botão de _reset_.

### 🎬 Vídeo 
https://github.com/wl11lm/TPSETicTacToe/assets/62153578/260df4a2-6efe-4cec-bc3f-d647b7efd8a7



## 👥 Autores
- Ana Livia da Silveira Lopes
- Cecilia Mourão Carvalhedo Mesquita
- William Marreiro Brito
