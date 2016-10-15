#ifndef CARRO_H
#define CARRO_H

#include "figuras.h"

#define LIMITE_ANGULO 45
#define BASE_WIDTH 0.35
#define BASE_HEIGHT 0.65
#define ACOPL_PROPORTION 0.6
#define ACOPL_WIDTH 0.08
#define ACOPL_HEIGHT 0.1
#define RODA_WIDTH 0.25
#define RODA_HEIGHT 0.1
#define CANHAO_WIDTH 0.1
#define CANHAO_HEIGHT 0.2

#define DIST_RANHURAS 0.5

class Tiro {
	private:
		Circulo* _circ;
		float _ang;
		float _velTiro;
	public:
		Tiro(Ponto p, double raio, double velTiro, double ang) {
			_circ = (Circulo*) malloc(sizeof(Circulo));
			_circ->centro = p;
			_circ->raio = raio;
			_circ->fill.r = 1.0;
			_circ->fill.g = 1.0;
			_circ->fill.b = 0.0;
			_velTiro = velTiro;
			_ang = ang;
		};
		~Tiro() {
			delete _circ;
		};
		Circulo getCirculo() {
			return *_circ;
		}
		void updateTiro(double time) {
			_circ->centro.x += _velTiro * time * cos(_ang * DEG2RAD);
			_circ->centro.y += _velTiro * time * sin(_ang * DEG2RAD);
		}
		void desenhar() {
			desenhaCirculo(_circ->raio, _circ->fill.r, _circ->fill.g, _circ->fill.b);
		};
};

class Carro {
	private:
		Circulo* _circ;
		float _velTiro;
		float _velCarro;
		float _angCarro;
		float _angCanhao;
		float _angRodas;
		float _posRanhuras;
		void desenhaAcopl(double width, double height, double proportion, int top, double angulo);

	public:
		Carro() {
			_circ = (Circulo*) malloc(sizeof(Circulo));
			_angCarro = 0;
			_angCanhao = 0;
			_angRodas = 0;
			_posRanhuras = DIST_RANHURAS/2;
		};
		~Carro() {
			delete _circ;
		};
		double getAngCanhao();
		void setPosicao(Ponto pos);
		void setRaio(float raio);
		void setVelTiro(float vtiro);
		void setVelCarro(float vcarro);
		Circulo getCirculo();
		void virarRoda(float ang, GLdouble timeDiff);
		void virarCanhao(float ang);
		Tiro* atirar();
		void andar(int direction, GLdouble timeDiff);
		void desenhar();
};

#endif


