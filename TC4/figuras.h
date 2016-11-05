#ifndef FIGURAS_H
#define FIGURAS_H

#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const double DEG2RAD = M_PI/180;

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

struct grupoCirculo {
	Circulo atual;
	struct grupoCirculo* prox;
};

typedef struct grupoCirculo GrupoCirculo;

struct retangulo {
	Ponto vEsqSup;
	double largura;
	double altura;
	Cor fill;
};

typedef struct retangulo Retangulo;

double distPonto(Ponto p1, Ponto p2);
bool dentroCirc(Circulo externo, Circulo interno);
bool colisaoCirc(Circulo c1, Circulo c2);
void desenhaRetangulo(double largura, double altura, double r, double g, double b);
void desenhaCirculo(double raio, double r, double g, double b);

#endif