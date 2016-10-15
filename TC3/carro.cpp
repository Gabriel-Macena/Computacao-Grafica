#include "carro.h"

void Carro::setPosicao(Ponto pos) {
	_circ->centro = pos;
}
void Carro::setRaio(float raio) {
	_circ->raio = raio;
}
void Carro::setVelTiro(float vtiro) {
	_velTiro = vtiro;
}
void Carro::setVelCarro(float vcarro) {
	_velCarro = vcarro;
}

double Carro::getAngCanhao() {
	return _angCanhao;
}

void Carro::desenhaAcopl(double width, double height, double proportion, int top, double angulo) {
	if (!top)
		top = -1;
	else
		top = 1;
	glPushMatrix();
	glTranslatef(width * -sin(angulo * DEG2RAD)/2, top*height*proportion/2, 0);
	glRotatef(angulo, 0, 0, 1);
	desenhaRetangulo(ACOPL_WIDTH, ACOPL_HEIGHT, 0.2, 0.6, 0.7);
	glTranslatef(0, ACOPL_HEIGHT, 0);
	glRotatef(_angRodas, 0, 0, 1);
	desenhaRetangulo(RODA_WIDTH, RODA_HEIGHT, 0.2, 0.6, 0.7);

	int direction = angulo/abs(angulo);

	//Desenha as ranhuras
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_LINES);
		glVertex2f(direction * _posRanhuras * RODA_WIDTH, 0);
		glVertex2f(direction * _posRanhuras * RODA_WIDTH, RODA_HEIGHT);
		glVertex2f(direction * (_posRanhuras - DIST_RANHURAS) * RODA_WIDTH, 0);
		glVertex2f(direction* (_posRanhuras - DIST_RANHURAS) * RODA_WIDTH, RODA_HEIGHT);
	glEnd();
	glPopMatrix();
}

Circulo Carro::getCirculo() {
	return *_circ;
}
void Carro::virarRoda(float ang, GLdouble timeDiff) {
	_angRodas += timeDiff * ang;
	if (_angRodas < -LIMITE_ANGULO) {
		_angRodas = -LIMITE_ANGULO;
	}
	else if (_angRodas > LIMITE_ANGULO) {
		_angRodas = LIMITE_ANGULO;
	}
}
void Carro::virarCanhao(float ang) {
	_angCanhao += ang;
	if (_angCanhao < -LIMITE_ANGULO) {
		_angCanhao = -LIMITE_ANGULO;
	}
	else if (_angCanhao > LIMITE_ANGULO) {
		_angCanhao = LIMITE_ANGULO;
	}
}
Tiro* Carro::atirar() {
	//Determina a posição do canhão
	const double transfMatrixCarro[][3] = {{cos(_angCarro * DEG2RAD), -sin(_angCarro * DEG2RAD), _circ->centro.x}, {sin(_angCarro * DEG2RAD), cos(_angCarro * DEG2RAD), _circ->centro.y}, {0.0, 0.0, 1.0}};
	const double origemCanhaoRelacaoCarro[3] = {0.0, BASE_HEIGHT * _circ->raio, 1.0};
	double origemCanhaoGlobal[3];
	int i,j;
	for (i = 0; i < 3; i++) {
		origemCanhaoGlobal[i] = 0.0;
		for (j = 0; j < 3; j++) {
			origemCanhaoGlobal[i] += transfMatrixCarro[i][j]*origemCanhaoRelacaoCarro[j];
		}
	} 


	//Determina a posição da saída do canhão
	double angTotal = _angCarro + _angCanhao;
	const double transfMatrix[][3] = {
										{cos(angTotal * DEG2RAD), -sin(angTotal * DEG2RAD), origemCanhaoGlobal[0]}, 
										{sin(angTotal * DEG2RAD), cos(angTotal * DEG2RAD), origemCanhaoGlobal[1]}, 
										{0.0, 0.0, 1.0}
									};
	const double saidaCanhaoRelacaoCanhao[3] = {0.0, CANHAO_HEIGHT * 2 * _circ->raio, 1.0};
	double saidaCanhaoGlobal[3];
	for (i = 0; i < 3; i++) {
		saidaCanhaoGlobal[i] = 0.0;
		for (j = 0; j < 3; j++) {
			saidaCanhaoGlobal[i] += transfMatrix[i][j]*saidaCanhaoRelacaoCanhao[j];
		}
	}

	Ponto p;
	p.x = saidaCanhaoGlobal[0];
	p.y = saidaCanhaoGlobal[1];

	return new Tiro(p, CANHAO_WIDTH * _circ->raio, _velTiro, angTotal + 90);

}
void Carro::andar(int direction, GLdouble timeDiff) {
	_angCarro += timeDiff * _angRodas/360;//_velCarro * (_angRodas/((2 - 0.3*M_PI)*LIMITE_ANGULO));
	if (_angCarro < 0) _angCarro = 360 + _angCarro;
	else if(_angCarro > 360) _angCarro = _angCarro - 360;
	_posRanhuras += direction * timeDiff * _velCarro/(2 * M_PI * RODA_WIDTH * _circ->raio);
	if (_posRanhuras > DIST_RANHURAS)
		_posRanhuras = 0;
	else if ((_posRanhuras - DIST_RANHURAS) < -DIST_RANHURAS)
		_posRanhuras = DIST_RANHURAS;
	_circ->centro.y += direction * timeDiff * _velCarro * cos(_angCarro*DEG2RAD);
	_circ->centro.x += direction * timeDiff * _velCarro * -sin(_angCarro*DEG2RAD);
}

void Carro::desenhar() {
	//O carro assume que o raio base é de 1
	glPushMatrix();
		//Escala ao círculo
		glScalef(_circ->raio*2, _circ->raio*2, 1);
		//Rotaciona o carro
		glRotatef(_angCarro, 0, 0, 1);
		//Desenha a base
		glPushMatrix();
			glTranslatef(0, -BASE_HEIGHT/2, 0);
			desenhaRetangulo(BASE_WIDTH, BASE_HEIGHT, 0.2, 0.9, 0.4);
		glPopMatrix();

		//Desenha os acoplamentos e as rodas

		//Canto inferior esquerdo
		desenhaAcopl(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 0, 90);

		//Canto superior esquerdo
		desenhaAcopl(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 1, 90);

		//Canto superior direito
		desenhaAcopl(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 1, -90);

		//Canto inferior direito
		desenhaAcopl(BASE_WIDTH, BASE_HEIGHT, ACOPL_PROPORTION, 0, -90);

		//Desenha o canhão
		glPushMatrix();
			glTranslatef(0, BASE_HEIGHT/2, 0);
			glRotatef(_angCanhao, 0, 0, 1);
			desenhaRetangulo(CANHAO_WIDTH, CANHAO_HEIGHT, 0.1, 0.7, 0.1);
		glPopMatrix();

	glPopMatrix();
}