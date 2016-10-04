#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/freeglut.h>
#include "tinyxml.h" 

struct quadrado {
	double x;
	double y;
	double lado;
	double r;
	double g;
	double b;
};

struct janela {
	char* titulo;
	double largura;
	double altura;
	double r;
	double g;
	double b;
};

typedef struct janela Janela;
typedef struct quadrado Quadrado;

Quadrado quad;
Janela jan;

//Usado pelo drag-and-drop para relativizar o movimento
double dx;
double dy;

//Determina se é para desenhar o quadrado ou não
bool desenhaQuad = false;
//Determina se o mouse está com o botão pressionado
bool mouseHold = false;


//Funcao que sera chamada toda vez que a janela for repintada.
void display() {
	//Limpa a tela com a cor especificada
	glClearColor(jan.r, jan.g, jan.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (desenhaQuad) {
		glColor3f(quad.r, quad.g, quad.b); //Quadrado na cor indicada
		glBegin(GL_QUADS);		
			//Desenha o quadrado nos vértices indicados
			glVertex2f(quad.x - quad.lado/2, quad.y - quad.lado/2); //Canto inferior esquerdo
			glVertex2f(quad.x + quad.lado/2, quad.y - quad.lado/2); //Canto inferior direito
			glVertex2f(quad.x + quad.lado/2,  quad.y + quad.lado/2); //Canto superior direito
			glVertex2f(quad.x - quad.lado/2,  quad.y + quad.lado/2); //Canto superior esquerdo
		glEnd();
	}
    //Desenha na tela
    glFlush();
}

//Funcao que determina se o mouse está dentro do quadrado
bool dentroQuad(double x, double y) {
	return (x >= (quad.x - quad.lado/2) && x <= (quad.x + quad.lado/2) && y >= (quad.y - quad.lado/2) && y <= (quad.y + quad.lado/2));
}

//Funcao que sera chamada toda vez que o mouse se mexer na tela quando tiver uma tecla apertada
void mouseDrag(int x, int y) {
	if (mouseHold) {
		//Reposiciona o quadrado
		quad.x = x - dx;
		quad.y = y - dy;
		//Atualiza a tela para refletir a nova posição
		glutPostRedisplay();
	}

}

//Funcao que sera chamada toda vez que uma tecla do mouse mudar de estado
void mouseControl(int button, int state, int x, int y) {
	//Relativiza a posição do mouse quanto à origem da janela
	if (button == GLUT_LEFT_BUTTON && !desenhaQuad)	{
		//Coloca o quadrado para desenhar na posição do mouse
		desenhaQuad = true;
		quad.x = x;
		quad.y = y;
		glutPostRedisplay();
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		//Bloqueia movimento do quadrado
		mouseHold = false;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && dentroQuad(x, y)) {
		//Permite movimento do quadrado
		if (!mouseHold) {
			mouseHold = true;
			dx = x - quad.x;
			dy = y - quad.y;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && desenhaQuad && dentroQuad(x, y)) {
		desenhaQuad = false;
		glutPostRedisplay();
	}
}

void init() {
	glClearColor(jan.r, jan.g, jan.b, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, jan.largura, jan.altura, 0.0, -1.0, 1.0);
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
		printf( "Falha na leitura do arquivo. Erro='%s'. \n", doc.ErrorDesc() );
		exit( 1 );
	}
	TiXmlNode* node = doc.RootElement(); //Busca o nó da raiz (aplicacao)
	TiXmlNode* janelaNode = node->FirstChild("janela"); //Recupera o nó da janela
	TiXmlElement* quadElement = node->FirstChildElement("quadrado"); //Recupera o nó do quadrado

	//Elementos da janela
	TiXmlElement* largJanelaElement = janelaNode->FirstChildElement("largura");
	TiXmlElement* altJanelaElement = janelaNode->FirstChildElement("altura");
	TiXmlElement* fundoJanelaElement = janelaNode->FirstChildElement("fundo");
	TiXmlElement* tituloJanelaElement = janelaNode->FirstChildElement("titulo");

	//Atributos da janela
	jan.largura = atof(largJanelaElement->GetText());
	jan.altura = atof(altJanelaElement->GetText());
	const char* titulo = tituloJanelaElement->GetText();
	jan.titulo = (char*) malloc(1+strlen(titulo));
	strcpy(jan.titulo, titulo);
	fundoJanelaElement->Attribute("corR", &jan.r);
	fundoJanelaElement->Attribute("corG", &jan.g);
	fundoJanelaElement->Attribute("corB", &jan.b);

	//Atributos do quadrado
	quadElement->Attribute("tamanho", &quad.lado);
	quadElement->Attribute("corR", &quad.r);
	quadElement->Attribute("corG", &quad.g);
	quadElement->Attribute("corB", &quad.b);

	free(arquivo);
}

int main(int argc, char** argv) {
	readXML(argv[1], "config.xml");
   	glutInit(&argc, argv);
   	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
   	glutInitWindowSize(jan.largura, jan.altura);
   	glutCreateWindow(jan.titulo); // Cria a janela com o título especificado
   	glutSetWindowTitle(jan.titulo);
   	glutReshapeWindow(jan.largura, jan.altura);   // Set the window's initial width & height
   	init();
   	glutMouseFunc(mouseControl); // Registre a função para captar clique do mouse (criação)
   	glutMotionFunc(mouseDrag); // Registre a função para controlar drag-and-drop do mouse
   	glutDisplayFunc(display); // Registre a função que desenha na tela
   	glutMainLoop();
   	return 0;
}
