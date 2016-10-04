#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/freeglut.h>
#include "tinyxml.h" 

#define PI 3.14159
const double DEG2RAD = PI/180;

struct ponto {
	double x;
	double y;
};

typedef struct ponto Ponto;

struct cor {
	double r;
	double g;
	double b;
};

typedef struct cor Cor;

struct circulo {
	Ponto centro;
	double raio;
	Cor fill;
};

typedef struct circulo Circulo;

struct retangulo {
	Ponto vEsqInf;
	double largura;
	double altura;
	Cor fill;
};

typedef struct retangulo Retangulo;

struct grupoCirculo {
	Circulo atual;
	struct grupoCirculo* prox;
};

typedef struct grupoCirculo GrupoCirculo;

Retangulo linha;
Circulo jogador;
Circulo pistaExterna;
Circulo pistaInterna;
GrupoCirculo* inimigos;

//Dimensões da janela
double janelaLarg;
double janelaAlt; 

//Controla o movimento no teclado
int keyState[256];

//Distância entre pontos
double distPonto(Ponto p1, Ponto p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

bool colisaoCirc(Circulo c1, Circulo c2) {
	return (distPonto(c1.centro, c2.centro) < (c1.raio + c2.raio));
}

bool colisaoInimigo() {
	GrupoCirculo* in = inimigos;
	while (in != NULL) {
		if (colisaoCirc(in->atual, jogador))
			return true;
		in = in->prox;
	}
	return false;
}

bool dentroCirc(Circulo externo, Circulo interno) {
	return (distPonto(externo.centro, interno.centro) <= abs(externo.raio - interno.raio));
}

void desenhaCirculo(Circulo circ) {
	glBegin(GL_POLYGON);
	int i = 0;
	for (i = 0; i <= 360; i++) {
		glVertex2f(circ.centro.x + cos(i*DEG2RAD)*circ.raio, circ.centro.y + sin(i*DEG2RAD)*circ.raio);
	}
	glEnd();
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

	//Desenha a pista externa
	glColor3f(pistaExterna.fill.r,pistaExterna.fill.g,pistaExterna.fill.b);
	desenhaCirculo(pistaExterna);

	//Desenha a pista interna
	glColor3f(pistaInterna.fill.r,pistaInterna.fill.g,pistaInterna.fill.b);
	desenhaCirculo(pistaInterna);	

	//Desenha a pista
	glColor3f(linha.fill.r,linha.fill.g,linha.fill.b);
	glBegin(GL_QUADS);
	glVertex2f(linha.vEsqInf.x, linha.vEsqInf.y);
	glVertex2f(linha.vEsqInf.x, linha.vEsqInf.y + linha.altura);
	glVertex2f(linha.vEsqInf.x + linha.largura, linha.vEsqInf.y + linha.altura);
	glVertex2f(linha.vEsqInf.x + linha.largura, linha.vEsqInf.y);
	glEnd();	

	//Desenha os círculos dos adversários
	GrupoCirculo* gc = inimigos;
	while(gc != NULL) {
		glColor3f(gc->atual.fill.r,gc->atual.fill.g,gc->atual.fill.b); //Circulo na cor vermelha
		desenhaCirculo(gc->atual);
		gc = gc->prox;
	}

	//Desenha o círculo do jogador
	glColor3f(jogador.fill.r,jogador.fill.g,jogador.fill.b);
	desenhaCirculo(jogador);	
	
    //Desenha na tela
    glutSwapBuffers();
}

void keyRelease(unsigned char key, int x, int y) {
	keyState[key] = 0;
}

void keyPress(unsigned char key, int x, int y) {
	keyState[key] = 1;
}

void idle(void) {
	Ponto previous = jogador.centro;
	//Movimenta o jogador
	if (keyState['W'] || keyState['w'])
		jogador.centro.y--;
	if (keyState['S'] || keyState['s'])
		jogador.centro.y++;
	if (keyState['A'] || keyState['a'])
		jogador.centro.x--;
	if (keyState['D'] || keyState['d'])
		jogador.centro.x++;
	//Testa se houve colisão
	if (colisaoInimigo() || colisaoCirc(pistaInterna, jogador) || !dentroCirc(pistaExterna, jogador))
		jogador.centro = previous; //retorna à posição original
	glutPostRedisplay();
}

void init() {
	glClearColor(1.0, 1.0, 1.0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(pistaExterna.centro.x - pistaExterna.raio, pistaExterna.centro.x + pistaExterna.raio, pistaExterna.centro.y + pistaExterna.raio, pistaExterna.centro.y - pistaExterna.raio, -1.0, 1.0);
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
				childElem->Attribute("cx", &jogador.centro.x);
				childElem->Attribute("cy", &jogador.centro.y);
				childElem->Attribute("r", &jogador.raio);
				jogador.fill = converterCor(strCor);
			}
		}
		else if(strcmp(tag, "rect") == 0) {
			childElem->Attribute("x", &linha.vEsqInf.x);
			childElem->Attribute("y", &linha.vEsqInf.y);
			childElem->Attribute("width", &linha.largura);
			childElem->Attribute("height", &linha.altura);
			linha.fill = converterCor(childElem->Attribute("fill"));
		}
	}

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
   	glutMainLoop();
   	return 0;
}
