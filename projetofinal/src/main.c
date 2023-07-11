// setenv app "setenv autoload no; setenv ipaddr 10.4.1.2; setenv serverip 10.4.1.1; tftp 0x80000000 appTimer.bin; echo *Booting to BareMetal *; go 0x80000000"

#include "uart.h"
#include "timer.h"
#include "interrupt.h"

//botoes
#define A1 		6
#define A2 		7
#define A3 		2
#define B1 		14
#define B2 		28
#define B3 		19
#define C1 		18
#define C2		16
#define C3		17

//leds azuis
#define AA1 	2
#define AA2 	5
#define AA3 	22
#define AB1 	23
#define AB2 	14
#define AB3 	12
#define AC1 	10
#define AC2		8
#define AC3		6

//leds vermelhos
#define VA1 	3
#define VA2 	4
#define VA3 	24 //?
#define VB1 	25
#define VB2 	15 //?
#define VB3 	13
#define VC1 	11
#define VC2		9
#define VC3		7

//char tabuleiro
char tabuleiro[3][3]={
	{' ', ' ', ' '},
	{' ', ' ', ' '},
	{' ', ' ', ' '},
};

char rodada = 'X';

void watchdogdisable();

void putCh(char c){
	while(!(HWREG(SOC_UART_0_REGS+UARTx_OP_R_LSR) & (1<<5)));

	HWREG(SOC_UART_0_REGS+UARTx_OP_W_THR) = c;
}

char getCh(){
	while(!(HWREG(HWREG(SOC_UART_0_REGS+UARTx_OP_R_LSR) & (1<<0))));

	return(HWREG(SOC_UART_0_REGS+UARTx_OP_W_THR));
}

int putString(char *str, unsigned int length){
	for(int i = 0; i < length; i++){
    	putCh(str[i]);
	}
	return(length);
}

int getString(char *buf, unsigned int length){
	for(int i = 0; i < length; i ++){
    	buf[i] = getCh();
   	}
	return(length);
}


void inverteRodada(){
	if(rodada == 'X') rodada = 'O';
	else rodada = 'X';
}

// Função para atualizar os LEDs com base no estado do tabuleiro
void atualizarLEDs() {
    // Percorre a matriz do tabuleiro e acende os LEDs correspondentes
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char estado = tabuleiro[i][j];
            if (estado == 'X') {
                // Acende o LED azul correspondente
                if (i == 0 && j == 0) {
                    gpioSetPinValue(GPIO2, AA1, HIGH);
                } else if (i == 0 && j == 1) {
                    gpioSetPinValue(GPIO2, AA2, HIGH);
                } else if (i == 0 && j == 2) {
                    gpioSetPinValue(GPIO2, AA3, HIGH);
                } else if (i == 1 && j == 0) {
                    gpioSetPinValue(GPIO2, AB1, HIGH);
                } else if (i == 1 && j == 1) {
                    gpioSetPinValue(GPIO2, AB2, HIGH);
                } else if (i == 1 && j == 2) {
                    gpioSetPinValue(GPIO2, AB3, HIGH);
                } else if (i == 2 && j == 0) {
                    gpioSetPinValue(GPIO2, AC1, HIGH);
                } else if (i == 2 && j == 1) {
                    gpioSetPinValue(GPIO2, AC2, HIGH);
                } else if (i == 2 && j == 2) {
                    gpioSetPinValue(GPIO2, AC3, HIGH);
                }
            } else if (estado == 'O') {
                // Acende o LED vermelho correspondente
                if (i == 0 && j == 0) {
                    gpioSetPinValue(GPIO2, VA1, HIGH);
                } else if (i == 0 && j == 1) {
                    gpioSetPinValue(GPIO2, VA2, HIGH);
                } else if (i == 0 && j == 2) {
                    gpioSetPinValue(GPIO2, VA3, HIGH);
                } else if (i == 1 && j == 0) {
                    gpioSetPinValue(GPIO2, VB1, HIGH);
                } else if (i == 1 && j == 1) {
                    gpioSetPinValue(GPIO2, VB2, HIGH);
                } else if (i == 1 && j == 2) {
                    gpioSetPinValue(GPIO2, VB3, HIGH);
                } else if (i == 2 && j == 0) {
                    gpioSetPinValue(GPIO2, VC1, HIGH);
                } else if (i == 2 && j == 1) {
                    gpioSetPinValue(GPIO2, VC2, HIGH);
                } else if (i == 2 && j == 2) {
                    gpioSetPinValue(GPIO2, VC3, HIGH);
                }
            }
        }
    }
}

// Função para verificar se há um vencedor
bool verificarVencedor(char jogador) {
    // Verifica todas as possibilidades de vitória
    for (int i = 0; i < 3; i++) {
        // Verifica as linhas
        if (tabuleiro[i][0] == jogador && tabuleiro[i][1] == jogador && tabuleiro[i][2] == jogador) {
            return true;
        }
        // Verifica as colunas
        if (tabuleiro[0][i] == jogador && tabuleiro[1][i] == jogador && tabuleiro[2][i] == jogador) {
            return true;
        }
    }
    // Verifica as diagonais
    if (tabuleiro[0][0] == jogador && tabuleiro[1][1] == jogador && tabuleiro[2][2] == jogador) {
        return true;
    }
    if (tabuleiro[0][2] == jogador && tabuleiro[1][1] == jogador && tabuleiro[2][0] == jogador) {
        return true;
    }
    return false;
}

void watchdogdisable(){		//To disable the timer, follow this sequence:
	HWREG(SOC_WDT_1_REGS+WDT_WSPR) = 0xAAAA;						//1. Write XXXX AAAAh in WDT_WSPR.
	while((HWREG(SOC_WDT_1_REGS+WDT_WWPS) & W_PEND_WSPR) != 0x0){}	//2. Poll for posted write to complete using WDT_WWPS.W_PEND_WSPR.
	HWREG(SOC_WDT_1_REGS+WDT_WSPR) = 0x5555;						//3. Write XXXX 5555h in WDT_WSPR.
	while((HWREG(SOC_WDT_1_REGS+WDT_WWPS) & W_PEND_WSPR) != 0x0){}	//4. Poll for posted write to complete using WDT_WWPS.W_PEND_WSPR.
}

int main(void){
	watchdogdisable();
	timerSetup();

	//Inicia o modulo
	gpioInitModule(GPIO1);
	gpioInitModule(GPIO2);

	//Seta o pino como gpio
	gpioPinMuxSetup(GPIO1, A1);
	gpioPinMuxSetup(GPIO1, A2);
	gpioPinMuxSetup(GPIO1, A3);
	gpioPinMuxSetup(GPIO1, B1);
	gpioPinMuxSetup(GPIO1, B2);
	gpioPinMuxSetup(GPIO1, B3);
	gpioPinMuxSetup(GPIO1, C1);
	gpioPinMuxSetup(GPIO1, C2);
	gpioPinMuxSetup(GPIO1, C3);
	gpioPinMuxSetup(GPIO2, AA1);
	gpioPinMuxSetup(GPIO2, AA2);
	gpioPinMuxSetup(GPIO2, AA3);
	gpioPinMuxSetup(GPIO2, AB1);
	gpioPinMuxSetup(GPIO2, AB2);
	gpioPinMuxSetup(GPIO2, AB3);
	gpioPinMuxSetup(GPIO2, AC1);
	gpioPinMuxSetup(GPIO2, AC2);
	gpioPinMuxSetup(GPIO2, AC3);
	gpioPinMuxSetup(GPIO2, VA1);
	gpioPinMuxSetup(GPIO2, VA2);
	gpioPinMuxSetup(GPIO2, VA3);
	gpioPinMuxSetup(GPIO2, VB1);
	gpioPinMuxSetup(GPIO2, VB2);
	gpioPinMuxSetup(GPIO2, VB3);
	gpioPinMuxSetup(GPIO2, VC1);
	gpioPinMuxSetup(GPIO2, VC2);
	gpioPinMuxSetup(GPIO2, VC3);


	//Seta como entrada e saida
	gpioSetDirection(GPIO1, A1, INPUT);
	gpioSetDirection(GPIO1, A2, INPUT);
	gpioSetDirection(GPIO1, A3, INPUT);
	gpioSetDirection(GPIO1, B1, INPUT);
	gpioSetDirection(GPIO1, B2, INPUT);
	gpioSetDirection(GPIO1, B3, INPUT);
	gpioSetDirection(GPIO1, C1, INPUT);
	gpioSetDirection(GPIO1, C2, INPUT);
	gpioSetDirection(GPIO1, C3, INPUT);
	gpioSetDirection(GPIO2, AA1, OUTPUT);
	gpioSetDirection(GPIO2, AA2, OUTPUT);
	gpioSetDirection(GPIO2, AA3, OUTPUT);
	gpioSetDirection(GPIO2, AB1, OUTPUT);
	gpioSetDirection(GPIO2, AB2, OUTPUT);
	gpioSetDirection(GPIO2, AB3, OUTPUT);
	gpioSetDirection(GPIO2, AC1, OUTPUT);
	gpioSetDirection(GPIO2, AC2, OUTPUT);
	gpioSetDirection(GPIO2, AC3, OUTPUT);
	gpioSetDirection(GPIO2, VA1, OUTPUT);
	gpioSetDirection(GPIO2, VA2, OUTPUT);
	gpioSetDirection(GPIO2, VA3, OUTPUT);
	gpioSetDirection(GPIO2, VB1, OUTPUT);
	gpioSetDirection(GPIO2, VB2, OUTPUT);
	gpioSetDirection(GPIO2, VB3, OUTPUT);
	gpioSetDirection(GPIO2, VC1, OUTPUT);
	gpioSetDirection(GPIO2, VC2, OUTPUT);
	gpioSetDirection(GPIO2, VC3, OUTPUT);

	//Desliga os pinos
	gpioSetPinValue(GPIO2, AA1, LOW);
	gpioSetPinValue(GPIO2, AA2, LOW);
	gpioSetPinValue(GPIO2, AA3, LOW);
	gpioSetPinValue(GPIO2, AB1, LOW);
	gpioSetPinValue(GPIO2, AB2, LOW);
	gpioSetPinValue(GPIO2, AB3, LOW);
	gpioSetPinValue(GPIO2, AC1, LOW);
	gpioSetPinValue(GPIO2, AC2, LOW);
	gpioSetPinValue(GPIO2, AC3, LOW);
	gpioSetPinValue(GPIO2, VA1, LOW);
	gpioSetPinValue(GPIO2, VA2, LOW);
	gpioSetPinValue(GPIO2, VA3, LOW);
	gpioSetPinValue(GPIO2, VB1, LOW);
	gpioSetPinValue(GPIO2, VB2, LOW);
	gpioSetPinValue(GPIO2, VB3, LOW);
	gpioSetPinValue(GPIO2, VC1, LOW);
	gpioSetPinValue(GPIO2, VC2, LOW);
	gpioSetPinValue(GPIO2, VC3, LOW);


	//Seta a interrupcao
	// gpioInterruptInit(GPIO1, 28, A);
	// gpioIrqInitBtn(GPIO1, 28);


	while(1){
		if (flagGet(GPIO1, A1)) {
			tabuleiro[0][0] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, A1);
    	}

		if (flagGet(GPIO1, A2)) {
			tabuleiro[0][1] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, A2);
		}

		if (flagGet(GPIO1, A3)) {
			tabuleiro[0][2] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, A3);
		}

		if (flagGet(GPIO1, B1)) {
			tabuleiro[1][0] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, B1);
		}

		if (flagGet(GPIO1, B2)) {
			tabuleiro[1][1] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, B2);
		}

		if (flagGet(GPIO1, B3)) {
			tabuleiro[1][2] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, B3);
		}

		if (flagGet(GPIO1, C1)) {
			tabuleiro[2][0] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, C1);
		}

		if (flagGet(GPIO1, C2)) {
			tabuleiro[2][1] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, C2);
		}

		if (flagGet(GPIO1, C3)) {
			tabuleiro[2][2] = rodada;
			inverteRodada();
			flagSetFalse(GPIO1, C3);
		}

		// Atualiza os LEDs com base no estado do tabuleiro
        atualizarLEDs();

		// Verifica se houve um vencedor
        if (verificarVencedor('X')) {
            gpioSetPinValue(GPIO2, AA1, LOW);
			gpioSetPinValue(GPIO2, AA2, LOW);
			gpioSetPinValue(GPIO2, AA3, LOW);
			gpioSetPinValue(GPIO2, AB1, LOW);
			gpioSetPinValue(GPIO2, AB2, LOW);
			gpioSetPinValue(GPIO2, AB3, LOW);
			gpioSetPinValue(GPIO2, AC1, LOW);
			gpioSetPinValue(GPIO2, AC2, LOW);
			gpioSetPinValue(GPIO2, AC3, LOW);
			gpioSetPinValue(GPIO2, VA1, LOW);
			gpioSetPinValue(GPIO2, VA2, LOW);
			gpioSetPinValue(GPIO2, VA3, LOW);
			gpioSetPinValue(GPIO2, VB1, LOW);
			gpioSetPinValue(GPIO2, VB2, LOW);
			gpioSetPinValue(GPIO2, VB3, LOW);
			gpioSetPinValue(GPIO2, VC1, LOW);
			gpioSetPinValue(GPIO2, VC2, LOW);
			gpioSetPinValue(GPIO2, VC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, HIGH);
			gpioSetPinValue(GPIO2, AA2, HIGH);
			gpioSetPinValue(GPIO2, AA3, HIGH);
			gpioSetPinValue(GPIO2, AB1, HIGH);
			gpioSetPinValue(GPIO2, AB2, HIGH);
			gpioSetPinValue(GPIO2, AB3, HIGH);
			gpioSetPinValue(GPIO2, AC1, HIGH);
			gpioSetPinValue(GPIO2, AC2, HIGH);
			gpioSetPinValue(GPIO2, AC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, LOW);
			gpioSetPinValue(GPIO2, AA2, LOW);
			gpioSetPinValue(GPIO2, AA3, LOW);
			gpioSetPinValue(GPIO2, AB1, LOW);
			gpioSetPinValue(GPIO2, AB2, LOW);
			gpioSetPinValue(GPIO2, AB3, LOW);
			gpioSetPinValue(GPIO2, AC1, LOW);
			gpioSetPinValue(GPIO2, AC2, LOW);
			gpioSetPinValue(GPIO2, AC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, HIGH);
			gpioSetPinValue(GPIO2, AA2, HIGH);
			gpioSetPinValue(GPIO2, AA3, HIGH);
			gpioSetPinValue(GPIO2, AB1, HIGH);
			gpioSetPinValue(GPIO2, AB2, HIGH);
			gpioSetPinValue(GPIO2, AB3, HIGH);
			gpioSetPinValue(GPIO2, AC1, HIGH);
			gpioSetPinValue(GPIO2, AC2, HIGH);
			gpioSetPinValue(GPIO2, AC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, LOW);
			gpioSetPinValue(GPIO2, AA2, LOW);
			gpioSetPinValue(GPIO2, AA3, LOW);
			gpioSetPinValue(GPIO2, AB1, LOW);
			gpioSetPinValue(GPIO2, AB2, LOW);
			gpioSetPinValue(GPIO2, AB3, LOW);
			gpioSetPinValue(GPIO2, AC1, LOW);
			gpioSetPinValue(GPIO2, AC2, LOW);
			gpioSetPinValue(GPIO2, AC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, HIGH);
			gpioSetPinValue(GPIO2, AA2, HIGH);
			gpioSetPinValue(GPIO2, AA3, HIGH);
			gpioSetPinValue(GPIO2, AB1, HIGH);
			gpioSetPinValue(GPIO2, AB2, HIGH);
			gpioSetPinValue(GPIO2, AB3, HIGH);
			gpioSetPinValue(GPIO2, AC1, HIGH);
			gpioSetPinValue(GPIO2, AC2, HIGH);
			gpioSetPinValue(GPIO2, AC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, LOW);
			gpioSetPinValue(GPIO2, AA2, LOW);
			gpioSetPinValue(GPIO2, AA3, LOW);
			gpioSetPinValue(GPIO2, AB1, LOW);
			gpioSetPinValue(GPIO2, AB2, LOW);
			gpioSetPinValue(GPIO2, AB3, LOW);
			gpioSetPinValue(GPIO2, AC1, LOW);
			gpioSetPinValue(GPIO2, AC2, LOW);
			gpioSetPinValue(GPIO2, AC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, HIGH);
			gpioSetPinValue(GPIO2, AA2, HIGH);
			gpioSetPinValue(GPIO2, AA3, HIGH);
			gpioSetPinValue(GPIO2, AB1, HIGH);
			gpioSetPinValue(GPIO2, AB2, HIGH);
			gpioSetPinValue(GPIO2, AB3, HIGH);
			gpioSetPinValue(GPIO2, AC1, HIGH);
			gpioSetPinValue(GPIO2, AC2, HIGH);
			gpioSetPinValue(GPIO2, AC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, AA1, LOW);
			gpioSetPinValue(GPIO2, AA2, LOW);
			gpioSetPinValue(GPIO2, AA3, LOW);
			gpioSetPinValue(GPIO2, AB1, LOW);
			gpioSetPinValue(GPIO2, AB2, LOW);
			gpioSetPinValue(GPIO2, AB3, LOW);
			gpioSetPinValue(GPIO2, AC1, LOW);
			gpioSetPinValue(GPIO2, AC2, LOW);
			gpioSetPinValue(GPIO2, AC3, LOW);
            break;
        } else if (verificarVencedor('O')) {
            gpioSetPinValue(GPIO2, AA1, LOW);
			gpioSetPinValue(GPIO2, AA2, LOW);
			gpioSetPinValue(GPIO2, AA3, LOW);
			gpioSetPinValue(GPIO2, AB1, LOW);
			gpioSetPinValue(GPIO2, AB2, LOW);
			gpioSetPinValue(GPIO2, AB3, LOW);
			gpioSetPinValue(GPIO2, AC1, LOW);
			gpioSetPinValue(GPIO2, AC2, LOW);
			gpioSetPinValue(GPIO2, AC3, LOW);
			gpioSetPinValue(GPIO2, VA1, LOW);
			gpioSetPinValue(GPIO2, VA2, LOW);
			gpioSetPinValue(GPIO2, VA3, LOW);
			gpioSetPinValue(GPIO2, VB1, LOW);
			gpioSetPinValue(GPIO2, VB2, LOW);
			gpioSetPinValue(GPIO2, VB3, LOW);
			gpioSetPinValue(GPIO2, VC1, LOW);
			gpioSetPinValue(GPIO2, VC2, LOW);
			gpioSetPinValue(GPIO2, VC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, HIGH);
			gpioSetPinValue(GPIO2, VA2, HIGH);
			gpioSetPinValue(GPIO2, VA3, HIGH);
			gpioSetPinValue(GPIO2, VB1, HIGH);
			gpioSetPinValue(GPIO2, VB2, HIGH);
			gpioSetPinValue(GPIO2, VB3, HIGH);
			gpioSetPinValue(GPIO2, VC1, HIGH);
			gpioSetPinValue(GPIO2, VC2, HIGH);
			gpioSetPinValue(GPIO2, VC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, LOW);
			gpioSetPinValue(GPIO2, VA2, LOW);
			gpioSetPinValue(GPIO2, VA3, LOW);
			gpioSetPinValue(GPIO2, VB1, LOW);
			gpioSetPinValue(GPIO2, VB2, LOW);
			gpioSetPinValue(GPIO2, VB3, LOW);
			gpioSetPinValue(GPIO2, VC1, LOW);
			gpioSetPinValue(GPIO2, VC2, LOW);
			gpioSetPinValue(GPIO2, VC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, HIGH);
			gpioSetPinValue(GPIO2, VA2, HIGH);
			gpioSetPinValue(GPIO2, VA3, HIGH);
			gpioSetPinValue(GPIO2, VB1, HIGH);
			gpioSetPinValue(GPIO2, VB2, HIGH);
			gpioSetPinValue(GPIO2, VB3, HIGH);
			gpioSetPinValue(GPIO2, VC1, HIGH);
			gpioSetPinValue(GPIO2, VC2, HIGH);
			gpioSetPinValue(GPIO2, VC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, LOW);
			gpioSetPinValue(GPIO2, VA2, LOW);
			gpioSetPinValue(GPIO2, VA3, LOW);
			gpioSetPinValue(GPIO2, VB1, LOW);
			gpioSetPinValue(GPIO2, VB2, LOW);
			gpioSetPinValue(GPIO2, VB3, LOW);
			gpioSetPinValue(GPIO2, VC1, LOW);
			gpioSetPinValue(GPIO2, VC2, LOW);
			gpioSetPinValue(GPIO2, VC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, HIGH);
			gpioSetPinValue(GPIO2, VA2, HIGH);
			gpioSetPinValue(GPIO2, VA3, HIGH);
			gpioSetPinValue(GPIO2, VB1, HIGH);
			gpioSetPinValue(GPIO2, VB2, HIGH);
			gpioSetPinValue(GPIO2, VB3, HIGH);
			gpioSetPinValue(GPIO2, VC1, HIGH);
			gpioSetPinValue(GPIO2, VC2, HIGH);
			gpioSetPinValue(GPIO2, VC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, LOW);
			gpioSetPinValue(GPIO2, VA2, LOW);
			gpioSetPinValue(GPIO2, VA3, LOW);
			gpioSetPinValue(GPIO2, VB1, LOW);
			gpioSetPinValue(GPIO2, VB2, LOW);
			gpioSetPinValue(GPIO2, VB3, LOW);
			gpioSetPinValue(GPIO2, VC1, LOW);
			gpioSetPinValue(GPIO2, VC2, LOW);
			gpioSetPinValue(GPIO2, VC3, LOW);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, HIGH);
			gpioSetPinValue(GPIO2, VA2, HIGH);
			gpioSetPinValue(GPIO2, VA3, HIGH);
			gpioSetPinValue(GPIO2, VB1, HIGH);
			gpioSetPinValue(GPIO2, VB2, HIGH);
			gpioSetPinValue(GPIO2, VB3, HIGH);
			gpioSetPinValue(GPIO2, VC1, HIGH);
			gpioSetPinValue(GPIO2, VC2, HIGH);
			gpioSetPinValue(GPIO2, VC3, HIGH);
			delay(200);
			gpioSetPinValue(GPIO2, VA1, LOW);
			gpioSetPinValue(GPIO2, VA2, LOW);
			gpioSetPinValue(GPIO2, VA3, LOW);
			gpioSetPinValue(GPIO2, VB1, LOW);
			gpioSetPinValue(GPIO2, VB2, LOW);
			gpioSetPinValue(GPIO2, VB3, LOW);
			gpioSetPinValue(GPIO2, VC1, LOW);
			gpioSetPinValue(GPIO2, VC2, LOW);
			gpioSetPinValue(GPIO2, VC3, LOW);

            break;
        }
	}

	return(0);
} /* ----------  end of function main  ---------- */