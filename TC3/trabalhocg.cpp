#include "tinyxml.h"
#include "carro.h"
#include <list>

using namespace std;

Retangulo linha;
Circulo pistaExterna;
Circulo pistaInterna;
GrupoCirculo* inimigos;
Carro* jogador = new Carro();
list<Tiro*> tiros;

//Dimensões da janela
double janelaLarg;
double janelaAlt; 

//Controla o movimento no teclado
int keyState[256];

bool colisaoInimigo() {
	GrupoCirculo* in = inimigos;
	while (in != NULL) {
		if (colisaoCirc(in->atual, jogador->getCirculo()))
			return true;
		in = in->prox;
	}
	return false;
}

Cor converterCor(const char* strCor) {
	Cor vCor;
	if (strcmp(strCor, "black") == 0) {
		vCor.r = 0.0;
		vCor.g = 0.0;
		vCor.b = 0.0;
		return vCor;
	}
	if (strcmp(strCor, "green") == 0) {
		vCor.r = 0.0;
		vCor.g = 1.0;
		vCor.b = 0.0;
		return vCor;
	}
	if (strcmp(strCor, "red") == 0) {
		vCor.r = 1.0;
		vCor.g = 0.0;
		vCor.b = 0.0;
		return vCor;
	}
	if (strcmp(strCor, "blue") == 0) {
		vCor.r = 0.0;
		vCor.g = 0.0;
		vCor.b = 1.0;
		return vCor;
	}
	if (strcmp(strCor, "white") == 0) {
		vCor.r = 1.0;
		vCor.g = 1.0;
		vCor.b = 1.0;
		return vCor;
	}
	vCor.r = 0.0;
	vCor.b = 0.0;
	vCor.g = 0.0;
	return vCor;
}

//Funcao que sera chamada toda vez que a janela for repintada.
void display() {
	//Limpa a tela com a cor especificada
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//double ref_x = pistaExterna.centro.x;
	//double ref_y = pistaExterna.centro.y;

	//Desenha a pista externa
	glPushMatrix();
	glTranslatef(pistaExterna.centro.x, pistaExterna.centro.y, 0);
	desenhaCirculo(pistaExterna.raio, pistaExterna.fill.r, pistaExterna.fill.g, pistaExterna.fill.b);
	glPopMatrix();

	//Desenha a pista interna
	glPushMatrix();
	glTranslatef(pistaInterna.centro.x, pistaInterna.centro.y, 0);
	desenhaCirculo(pistaInterna.raio, pistaInterna.fill.r, pistaInterna.fill.g, pistaInterna.fill.b);
	glPopMatrix();

	//Desenha a pista
	glPushMatrix();
	glTranslatef(linha.vEsqSup.x + linha.largura/2, linha.vEsqSup.y - linha.altura, 0);
	desenhaRetangulo(linha.largura, linha.altura, linha.fill.r, linha.fill.g, linha.fill.b);
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
	glPushMatrix();
	glTranslatef(jogador->getCirculo().centro.x, jogador->getCirculo().centro.y, 0);
	//desenhaCirculo(jogador->getCirculo().raio, 0.0, 1.0, 0.0);
	jogador->desenhar();
	glPopMatrix();

	//Desenha os círculos dos adversários
	GrupoCirculo* gc = inimigos;
	while(gc != NULL) {
		glPushMatrix();
		glTranslatef(gc->atual.centro.x, gc->atual.centro.y, 0);		
		desenhaCirculo(gc->atual.raio, gc->atual.fill.r, gc->atual.fill.g, gc->atual.fill.b);
		glPopMatrix();
		gc = gc->prox;
	}

    //Desenha na tela
    glutSwapBuffers();
}

void keyRelease(unsigned char key, int x, int y) {
	keyState[key] = 0;
}

void keyPress(unsigned char key, int x, int y) {
	keyState[key] = 1;
}

int outOfBounds(Ponto p) {
	return p.x > pistaExterna.raio || p.x < -pistaExterna.raio || p.y > pistaExterna.raio || p.y < -pistaExterna.raio;
}

void idle(void) {
	static GLdouble previousTime = 0;
    GLdouble currentTime;
    GLdouble timeDiference;
    
    // Elapsed time from the initiation of the game.
    currentTime = glutGet(GLUT_ELAPSED_TIME);
    timeDiference = currentTime - previousTime; // Elapsed time from the previous frame.
    previousTime = currentTime; //Update previous time

	Ponto previous = jogador->getCirculo().centro;
	//Movimenta o jogador
	if (keyState['W'] || keyState['w'])
		jogador->andar(1, timeDiference);
	if (keyState['S'] || keyState['s'])
		jogador->andar(-1, timeDiference);
	if (keyState['A'] || keyState['a'])
		jogador->virarRoda(0.1, timeDiference);
	if (keyState['D'] || keyState['d'])
		jogador->virarRoda(-0.1, timeDiference);
	//Testa se houve colisão
	if (colisaoInimigo() || colisaoCirc(pistaInterna, jogador->getCirculo()) || !dentroCirc(pistaExterna, jogador->getCirculo()))
		jogador->setPosicao(previous); //retorna à posição original

	for(list<Tiro*>::iterator it = tiros.begin(); it != tiros.end(); ++it) {
		Tiro* t = *it;
		t->updateTiro(timeDiference);
		if (outOfBounds(t->getCirculo().centro)) {
			it = tiros.erase(it);
			delete t;
		}
	}

	glutPostRedisplay();
}

void mouseDrag(int x, int y) {
	static int lastMouseX = pistaExterna.raio;
	jogador->virarCanhao(2 * (x < lastMouseX) - (lastMouseX < x));
	lastMouseX = x;
}

void mousePress(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//cria o tiro
		tiros.push_back(jogador->atirar());
	}
}

void init() {
	glClearColor(1.0, 1.0, 1.0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-pistaExterna.raio, pistaExterna.raio, -pistaExterna.raio, pistaExterna.raio, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void relativizarPontos() {
	
	double relx = pistaExterna.centro.x;
	double rely = pistaExterna.centro.y;

	//Posiciona a pista interna
	pistaInterna.centro.x = pistaInterna.centro.x - relx;
	pistaInterna.centro.y = rely - pistaInterna.centro.y;

	//Posiciona os inimigos
	GrupoCirculo* gc = inimigos;
	while(gc != NULL) {
		gc->atual.centro.x = gc->atual.centro.x - relx;
		gc->atual.centro.y = rely - gc->atual.centro.y;
		gc = gc->prox;
	}

	//Posiciona o jogador
	Ponto p = jogador->getCirculo().centro;
	p.x = p.x - relx;
	p.y = rely - p.y;
	jogador->setPosicao(p);

	//Posiciona a largada
	linha.vEsqSup.x = linha.vEsqSup.x - relx;
	linha.vEsqSup.y = rely - linha.vEsqSup.y;

	//Centraliza a pista externa à origem
	pistaExterna.centro.x = 0.0;
	pistaExterna.centro.y = 0.0;
}

void readSVG(char* path, char* file) {
	char* arquivo = (char*) malloc(1+strlen(path)+strlen(file));
	strcpy(arquivo, path);
	//Concatena o endereco da pasta ao nome do arquivo
	arquivo = strcat(arquivo, file);
   	TiXmlDocument doc(arquivo);
	bool loadOkay = doc.LoadFile();
	
	if ( !loadOkay )
	{
		printf( "Falha na leitura do arquivo SVG. Erro='%s'. \n", doc.ErrorDesc() );
		exit( 1 );
	}
	TiXmlNode* node = doc.RootElement(); //Busca o nó da raiz (svg)
	TiXmlNode* child = 0;
	while (child = node->IterateChildren(child)) {
		TiXmlElement* childElem = child->ToElement();
		const char* tag = childElem->Value();
		if (strcmp(tag, "circle") == 0) {
			const char* strCor = childElem->Attribute("fill");
			if (strcmp(strCor, "blue") == 0) {
				//Pista externa
				childElem->Attribute("cx", &pistaExterna.centro.x);
				childElem->Attribute("cy", &pistaExterna.centro.y);
				childElem->Attribute("r", &pistaExterna.raio);
				pistaExterna.fill = converterCor(strCor);
			}
			else if (strcmp(strCor, "white") == 0) {
				//Pista interna
				childElem->Attribute("cx", &pistaInterna.centro.x);
				childElem->Attribute("cy", &pistaInterna.centro.y);
				childElem->Attribute("r", &pistaInterna.raio);
				pistaInterna.fill = converterCor(strCor);
			}
			else if (strcmp(strCor, "red") == 0) {
				//Inimigo
				GrupoCirculo* novoInimigo = (GrupoCirculo*) malloc(sizeof(GrupoCirculo));
				novoInimigo->prox = NULL;
				Circulo gc;
				childElem->Attribute("cx", &gc.centro.x);
				childElem->Attribute("cy", &gc.centro.y);
				childElem->Attribute("r", &gc.raio);
				gc.fill = converterCor(strCor);
				novoInimigo->atual = gc;
				if (inimigos == NULL)
					inimigos = novoInimigo;
				else {
					GrupoCirculo* in = inimigos;
					while (in->prox != NULL)
						in = in->prox;
					in->prox = novoInimigo;
				}
			}
			else if (strcmp(strCor, "green") == 0) {
				//Jogador
				Ponto p;
				double raio;
				childElem->Attribute("cx", &p.x);
				childElem->Attribute("cy", &p.y);
				childElem->Attribute("r", &raio);
				jogador->setPosicao(p);
				jogador->setRaio(raio);
			}
		}
		else if(strcmp(tag, "rect") == 0) {
			childElem->Attribute("x", &linha.vEsqSup.x);
			childElem->Attribute("y", &linha.vEsqSup.y);
			childElem->Attribute("width", &linha.largura);
			childElem->Attribute("height", &linha.altura);
			linha.fill = converterCor(childElem->Attribute("fill"));
		}
	}

	relativizarPontos();

	free(arquivo);
}

void readXML(char* path, const char* file) {
	char* arquivo = (char*) malloc(1+strlen(path)+strlen(file));
	strcpy(arquivo, path);
	//Concatena o endereco da pasta ao nome do arquivo
	arquivo = strcat(arquivo, file);
   	TiXmlDocument doc(arquivo);
	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
	{
		printf( "Falha na leitura do arquivo XML. Erro='%s'. \n", doc.ErrorDesc() );
		exit( 1 );
	}
	TiXmlNode* node = doc.RootElement(); //Busca o nó da raiz (aplicacao)
	TiXmlElement* arquivoDaArena = node->FirstChildElement("arquivoDaArena"); //Recupera o nó do arquivo
	TiXmlElement* attrCarro = node->FirstChildElement("carro"); //Recupera o nó do carro

	//Atributos do arquivo
	const char* nome = arquivoDaArena->Attribute("nome");
	const char* tipo = arquivoDaArena->Attribute("tipo");
	const char* cam = arquivoDaArena->Attribute("caminho");

	char* caminho = (char*) malloc(1+strlen(cam));
	strcpy(caminho, cam);

	char* arq = (char*) malloc(1+strlen(nome)+1+strlen(tipo));
	strcpy(arq, nome);
	arq = strcat(arq, ".");
	arq = strcat(arq, tipo);
	readSVG(caminho, arq);

	//Atributos do carro
	jogador->setVelTiro(atof(attrCarro->Attribute("velTiro")));
	jogador->setVelCarro(atof(attrCarro->Attribute("velCarro")));

	free(arq);
	free(arquivo);
}

int main(int argc, char** argv) {
	inimigos = NULL;
	readXML(argv[1], "config.xml");
	janelaLarg = pistaExterna.raio*2;
	janelaAlt = pistaExterna.raio*2;
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
