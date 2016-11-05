#ifndef CARRO_H
#define CARRO_H

#include "figuras.h"
#include <list>

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

typedef struct {
	double inimigoFireRate;
	double inimigoVelCarro;
	double inimigoVelTiro;
} EnemyAttr;

class Carro;
class CarroInimigo;

class Tiro {
	private:
		Circulo* _circ;
		float _ang;
		float _velTiro;
		bool _isPlayerShot;
	public:
		Tiro(Ponto p, double raio, double velTiro, double ang) {
			_circ = (Circulo*) malloc(sizeof(Circulo));
			_circ->centro = p;
			_circ->raio = raio;
			_velTiro = velTiro;
			_ang = ang;
		};
		~Tiro() {
			delete _circ;
		};
		Circulo getCirculo();
		Ponto getPosicao();
		bool isPlayerShot();
		void setPlayerShot(bool playerShot);
		bool colisaoCarro(Carro* carro);
		void updateTiro(double time);
		void desenhar();
};

class Carro {
	protected:
		Circulo* _circ;
		double _velTiro;
		double _velCarro;
		double _angCarro;
		double _angCanhao;
		double _angRodas;
		double _posRanhuras;
		bool _isPlayer;
		double corChassis[3];
		double corRodas[3];
		double corCanhao[3];

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
		}
		bool isPlayer() {
			return _isPlayer;
		};
		double getAngCanhao();
		Circulo getCirculo();
		Ponto getPosicao();
		double getVelCarro();
		double getTurnRate();
		bool colisaoCarro(Carro* self, std::list<CarroInimigo*>& carros);
		void setAngRodas(double ang);
		void setPosicao(Ponto pos);
		void setRaio(double raio);
		void setVelTiro(double vtiro);
		void setVelCarro(double vcarro);		
		void virarRoda(double ang);
		void virarCanhao(double ang);
		void virarCarro(double taxa, GLdouble timeDiff);
		void moverRanhuras(int direction, GLdouble timeDiff);
		Tiro* atirar();
		void desenhar();
};

class CarroJogador : public Carro {
	public:
		CarroJogador() : Carro() {
			corChassis[0] = 0.2; 
			corChassis[1] = 0.9; 
			corChassis[2] = 0.4;
			corRodas[0] = 0.2; 
			corRodas[1] = 0.6;
			corRodas[2] =  0.7;
			corCanhao[0] = 0.1;
			corCanhao[1] = 0.7; 
			corCanhao[2] = 0.1;
			_isPlayer = true;
		}
		Tiro* atirar();
		void andar(int direction, GLdouble timeDiff);
};

class CarroInimigo : public Carro {
	private:
		double _fireRate;
		double _fireTime;
		double _raioMovimento;
		double _reverseFlag;
		double _reverseTime;
	public:
		CarroInimigo() : Carro() {
			corChassis[0] = 0.9; 
			corChassis[1] = 0.2; 
			corChassis[2] = 0.2;
			corRodas[0] = 0.7; 
			corRodas[1] = 0.4;
			corRodas[2] =  0.2;
			corCanhao[0] = 0.7;
			corCanhao[1] = 0.1; 
			corCanhao[2] = 0.1;
			_fireTime = 0;
			_isPlayer = false;
			_reverseFlag = 1;
			_reverseTime = 0;
		}
		void reverterMovimento() {
			_reverseTime = 0;
			_reverseFlag = -_reverseFlag;
		}

		double getTempoReverso() {
			return _reverseTime;
		}

		double getDirecaoMovimento() {
			return _reverseFlag;
		}

		double getRaioMovimento() {
			return _raioMovimento;
		}
		void setRaioMovimento(double r) {
			_raioMovimento = r;
		}
		void setFireRate(double fireRate) {
			_fireRate = fireRate;
		}
		double getFireRate() {
			return _fireRate;
		}
		void alinharAngulo();
		void atirar(std::list<Tiro*>& listTiros, GLdouble timeDiff);
		Ponto andar(GLdouble timeDiff);
		void updatePosition();
};

#endif


