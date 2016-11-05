#include "figuras.h"

//Distância entre pontos
double distPonto(Ponto p1, Ponto p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

bool dentroCirc(Circulo externo, Circulo interno) {
	return (distPonto(externo.centro, interno.centro) <= abs(externo.raio - interno.raio));
}


bool colisaoCirc(Circulo c1, Circulo c2) {
	return (distPonto(c1.centro, c2.centro) < (c1.raio + c2.raio));
}

void desenhaRetangulo(double largura, double altura, double r, double g, double b) {
	glColor3f(r,g,b);
	glBegin(GL_QUADS);
	glVertex2f(-largura/2.0, 0);
	glVertex2f(-largura/2.0, altura);
	glVertex2f(largura/2.0, altura);
	glVertex2f(largura/2.0, 0);
	glEnd();
}

void desenhaCirculo(double raio, double r, double g, double b) {
	glColor3f(r,g,b);
	glBegin(GL_POLYGON);
	int i = 0;
	for (i = 0; i <= 360; i++) {
		glVertex2f(cos(i*DEG2RAD)*raio, sin(i*DEG2RAD)*raio);
	}
	glEnd();
}