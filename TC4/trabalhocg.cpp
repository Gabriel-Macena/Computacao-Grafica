#include "leitor.h"

using namespace std;

Retangulo* linha = (Retangulo*) malloc(sizeof(Retangulo));
Circulo* pistaExterna = (Circulo*) malloc(sizeof(Circulo));
Circulo* pistaInterna = (Circulo*) malloc(sizeof(Circulo));
list<CarroInimigo*> inimigos;
CarroJogador* jogador = new CarroJogador();
list<Tiro*> tiros;

//Dimensões da janela
double janelaLarg;
double janelaAlt;

//Atributos dos inimigos
EnemyAttr* enemyAttributes;

//Flags de controle do jogo
bool gameStart = false;
bool gameWon = false;
bool gameOver = false;
int currentCheckpoint = 0;

//Controla o movimento no teclado
int keyState[256];

// Text variable
static char str[2000];
void * font = GLUT_BITMAP_9_BY_15;

//Timer
double timerSeg = 0;
double timerMin = 0;
double timerHour = 0;
GLdouble previousTime = 0;
GLdouble startTime = 0;

void printTimer(GLfloat x, GLfloat y)
{
    //Create a string to be printed
    char *tmpStr;
    sprintf(str, "Tempo: %02d:%02d:%02d", (int) timerHour, (int) timerMin, (int) timerSeg);
    //Define the position to start printing
    glRasterPos2f(x, y);
    //Print  the first Char with a certain font
    //glutBitmapLength(font,(unsigned char*)str);
    tmpStr = str;
    //Print each of the other Char at time
    while( *tmpStr ){
        glutBitmapCharacter(font, *tmpStr);
        tmpStr++;
    }

}

void printEnd(GLfloat x, GLfloat y)
{
    //Create a string to be printed
    char *tmpStr;
    if (gameWon)
    	sprintf(str, "Voce ganhou.");
    else
    	sprintf(str, "Voce perdeu.");
    //Define the position to start printing
    glRasterPos2f(x, y);
    //Print  the first Char with a certain font
    //glutBitmapLength(font,(unsigned char*)str);
    tmpStr = str;
    //Print each of the other Char at time
    while( *tmpStr ){
        glutBitmapCharacter(font, *tmpStr);
        tmpStr++;
    }

}

//Funcao que sera chamada toda vez que a janela for repintada.
void display() {
	//Limpa a tela com a cor especificada
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Desenha a pista externa
	glPushMatrix();
	glTranslatef(pistaExterna->centro.x, pistaExterna->centro.y, 0);
	desenhaCirculo(pistaExterna->raio, pistaExterna->fill.r, pistaExterna->fill.g, pistaExterna->fill.b);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna->centro.x, pistaInterna->centro.y, 0);
	desenhaCirculo(pistaInterna->raio, pistaInterna->fill.r, pistaInterna->fill.g, pistaInterna->fill.b);
	glPopMatrix();

	//Desenha a pista
	glPushMatrix();
	glTranslatef(linha->vEsqSup.x + linha->largura/2, linha->vEsqSup.y - linha->altura, 0);
	desenhaRetangulo(linha->largura, linha->altura, linha->fill.r, linha->fill.g, linha->fill.b);
	glPopMatrix();

	//Desenha o tiro
	for(list<Tiro*>::iterator it = tiros.begin(); it != tiros.end(); ++it) {
		Tiro* t = *it;
		glPushMatrix();
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
		t->desenhar();
		glPopMatrix();		
	}

	//Desenha o jogador
	if (jogador != NULL) {
		glPushMatrix();
		glTranslatef(jogador->getCirculo().centro.x, jogador->getCirculo().centro.y, 0);
		jogador->desenhar();
		glPopMatrix();
	}

	//Desenha os círculos dos adversários
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* t = *it;
		glPushMatrix();
		glTranslatef(t->getCirculo().centro.x, t->getCirculo().centro.y, 0);
		t->desenhar();
		glPopMatrix();		
	}

	//Desenha o tempo na tela
	glColor3f(0.0, 0.0, 0.0);
	printTimer(janelaLarg / 2  - 150.0, janelaAlt / 2 - 15.0f);

	if (gameOver)
		printEnd(-45.0f, 0);

    //Desenha na tela
    glutSwapBuffers();
}

void keyRelease(unsigned char key, int x, int y) {
	keyState[key] = 0;
}

void keyPress(unsigned char key, int x, int y) {
	keyState[key] = 1;
	if (!gameStart && (key == 'W' || key == 'w')) {
		gameStart = true;
		startTime = previousTime;
	}
}

int outOfBounds(Ponto p) {
	return p.x > pistaExterna->raio || p.x < -pistaExterna->raio || p.y > pistaExterna->raio || p.y < -pistaExterna->raio;
}

void gameRun(GLdouble currentTime, GLdouble timeDifference) {
	double time = currentTime - startTime;
    timerSeg = (int) (time / 1000) % 60;
    timerMin = (int) (time / 1000) / 60;
    timerHour = (int) (time / 1000) / 3600;
    Ponto previousPos;
    if (jogador != NULL) {
		previousPos = jogador->getPosicao();
		int direction = 0;
		//Movimenta o jogador
		if (keyState['W'] || keyState['w']) {
			direction = 1;
			jogador->andar(direction, timeDifference);
		}
		if (keyState['S'] || keyState['s']) {
			direction = -1;
			jogador->andar(direction, timeDifference);
		}
		if (keyState['A'] || keyState['a'])
			jogador->virarRoda(1.5);
		if (keyState['D'] || keyState['d'])
			jogador->virarRoda(-1.5);
		//Testa se houve colisão
		if (jogador->colisaoCarro(jogador, inimigos) || colisaoCirc(*pistaInterna, jogador->getCirculo()) || !dentroCirc(*pistaExterna, jogador->getCirculo()))
			jogador->setPosicao(previousPos); //retorna à posição original
		else
			jogador->virarCarro(jogador->getTurnRate() * direction, timeDifference);
	}

	//Atualiza os tiros e confere se algum carro foi acertado
	for(list<Tiro*>::iterator it = tiros.begin(); it != tiros.end(); ++it) {
		Tiro* t = *it;
		t->updateTiro(timeDifference);
		if (outOfBounds(t->getPosicao())) {
			it = tiros.erase(it);
			delete t;
		}
		else if (jogador != NULL && t->colisaoCarro(jogador)) {
			it = tiros.erase(it);
			delete t;
			delete jogador;
			jogador = NULL;
			gameOver = true;
		}
		else {
			for(list<CarroInimigo*>::iterator its = inimigos.begin(); its != inimigos.end(); ++its) {
				CarroInimigo* ci = *its;
				if (t->colisaoCarro(ci)) {
					its = inimigos.erase(its);
					delete ci;
					it = tiros.erase(it);
					delete t;
					break;
				}
			}
		}
	}


	//Movimento e tiro automático dos inimigos
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* ci = *it;
		ci->atirar(tiros, timeDifference);
		Ponto antigo = ci->getPosicao();
		ci->andar(timeDifference);
		if (jogador != NULL && colisaoCirc(jogador->getCirculo(), ci->getCirculo())) {
				ci->reverterMovimento();
			ci->setPosicao(antigo);
		}
		else if (ci->colisaoCarro(ci, inimigos) || colisaoCirc(*pistaInterna, ci->getCirculo()) || !dentroCirc(*pistaExterna, ci->getCirculo())) {
			ci->reverterMovimento();
			ci->setPosicao(antigo);
		}
		else {
			if (ci->getDirecaoMovimento() < 0 && ci->getTempoReverso() >= 1) {
				ci->reverterMovimento();
				ci->alinharAngulo();
			}
			ci->virarCarro(ci->getDirecaoMovimento() * ci->getTurnRate(), timeDifference);
		}
	}

	if (jogador != NULL) {
		//Confere se o jogador atingiu um checkpoint
		Ponto currentPos = jogador->getPosicao();
		switch(currentCheckpoint) {
			case 0:
				//Topo da pista
				if (previousPos.x >= 0 && currentPos.x <= 0 && previousPos.y >= 0)
					currentCheckpoint++;
				break;
			case 1:
				//Lateral esquerda da pista
				if (previousPos.y >= 0 && currentPos.y <= 0 && previousPos.x <= 0)
					currentCheckpoint++;
				break;
			case 2:
				//Fundo da pista
				if (previousPos.x <= 0 && currentPos.x >= 0 && previousPos.y <= 0)
					currentCheckpoint++;
				break;
			case 3:
				//Lateral direita da pista
				if (previousPos.y <= 0 && currentPos.y >= 0 && previousPos.x >= 0) {
					gameWon = true;
					gameOver = true;
				}
				break;
		}
	}
}

void idle(void) {
    GLdouble currentTime;
    GLdouble timeDifference;

    // Elapsed time from the initiation of the game.
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeDifference = currentTime - previousTime; // Elapsed time from the previous frame.
    previousTime = currentTime; //Update previous time
	if (gameStart && !gameOver)
		gameRun(currentTime, timeDifference);
	glutPostRedisplay();
}

void mouseDrag(int x, int y) {
	static int lastMouseX = 0;
	if (jogador != NULL && gameStart && !gameOver)
		jogador->virarCanhao( 2 * ((x < lastMouseX) - (lastMouseX < x)) );
	lastMouseX = x;
}

void mousePress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//cria o tiro
		if (jogador != NULL && gameStart && !gameOver)
			tiros.push_back(jogador->atirar());
	}
}

void init() {
	glClearColor(1.0, 1.0, 1.0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-pistaExterna->raio, pistaExterna->raio, -pistaExterna->raio, pistaExterna->raio, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {
	readXML(argv[1], "config.xml", enemyAttributes, jogador, inimigos, pistaInterna, pistaExterna, linha);
	janelaLarg = pistaExterna->raio * 2;
	janelaAlt = pistaExterna->raio * 2;
   	glutInit(&argc, argv);
   	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   	glutInitWindowSize(janelaLarg, janelaAlt);
   	glutCreateWindow("Pista"); // Cria a janela com o título especificado
   	glutReshapeWindow(janelaLarg, janelaAlt);   // Set the window's initial width & height
   	init();
   	glutDisplayFunc(display); // Registre a função que desenha na tela
   	glutKeyboardFunc(keyPress);
   	glutKeyboardUpFunc(keyRelease);
   	glutIdleFunc(idle);
   	glutMouseFunc(mousePress);
   	glutPassiveMotionFunc(mouseDrag);
   	glutMotionFunc(mouseDrag);
   	glutMainLoop();
   	return 0;
}
