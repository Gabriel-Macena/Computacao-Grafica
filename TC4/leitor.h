#ifndef LEITOR_H
#define LEITOR_H

#include "tinyxml.h"
#include "carro.h"
#include <list>

using namespace std;

void readXML(char* path, const char* file, EnemyAttr* enemyAttr, CarroJogador* jogador, list<CarroInimigo*>& inimigos, Circulo* pistaInterna, Circulo* pistaExterna, Retangulo* linha);
void readSVG(char* path, char* file, EnemyAttr* enemyAttr, CarroJogador* jogador, list<CarroInimigo*>& inimigos, Circulo* pistaInterna, Circulo* pistaExterna, Retangulo* linha);
void relativizarPontos(CarroJogador* jogador, list<CarroInimigo*>& inimigos, Circulo* pistaInterna, Circulo* pistaExterna, Retangulo* linha);
Cor converterCor(const char* strCor);

#endif