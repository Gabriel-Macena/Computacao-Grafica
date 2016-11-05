#include "leitor.h"

using namespace std;

Cor converterCor(const char* strCor) {
	Cor vCor;
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
	vCor.g = 0.0;
	vCor.b = 0.0;
	return vCor;
}

void relativizarPontos(CarroJogador* jogador, list<CarroInimigo*>& inimigos, Circulo* pistaInterna, Circulo* pistaExterna, Retangulo* linha) {
	
	double relx = pistaExterna->centro.x;
	double rely = pistaExterna->centro.y;

	//Posiciona a pista interna
	pistaInterna->centro.x = pistaInterna->centro.x - relx;
	pistaInterna->centro.y = rely - pistaInterna->centro.y;

	//Posiciona os inimigos
	for(list<CarroInimigo*>::iterator it = inimigos.begin(); it != inimigos.end(); ++it) {
		CarroInimigo* ci = *it;
		Ponto p = ci->getCirculo().centro;
		p.x = p.x - relx;
		p.y = rely - p.y;
		ci->setPosicao(p);
		ci->alinharAngulo();
	}

	//Posiciona o jogador
	Ponto p = jogador->getCirculo().centro;
	p.x = p.x - relx;
	p.y = rely - p.y;
	jogador->setPosicao(p);

	//Posiciona a largada
	linha->vEsqSup.x = linha->vEsqSup.x - relx;
	linha->vEsqSup.y = rely - linha->vEsqSup.y;

	//Centraliza a pista externa à origem
	pistaExterna->centro.x = 0.0;
	pistaExterna->centro.y = 0.0;
}

void readSVG(char* path, char* file, EnemyAttr* enemyAttr, CarroJogador* jogador, list<CarroInimigo*>& inimigos, Circulo* pistaInterna, Circulo* pistaExterna, Retangulo* linha){
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
				childElem->Attribute("cx", &pistaExterna->centro.x);
				childElem->Attribute("cy", &pistaExterna->centro.y);
				childElem->Attribute("r", &pistaExterna->raio);
				pistaExterna->fill = converterCor(strCor);
			}
			else if (strcmp(strCor, "white") == 0) {
				//Pista interna
				childElem->Attribute("cx", &pistaInterna->centro.x);
				childElem->Attribute("cy", &pistaInterna->centro.y);
				childElem->Attribute("r", &pistaInterna->raio);
				pistaInterna->fill = converterCor(strCor);
			}
			else if (strcmp(strCor, "red") == 0) {
				//Inimigo
				CarroInimigo* ci = new CarroInimigo();
				Ponto p;
				double raio;
				childElem->Attribute("cx", &p.x);
				childElem->Attribute("cy", &p.y);
				childElem->Attribute("r", &raio);
				ci->setPosicao(p);
				ci->setRaio(raio);
				ci->setFireRate(enemyAttr->inimigoFireRate);
				ci->setVelTiro(enemyAttr->inimigoVelTiro);
				ci->setVelCarro(enemyAttr->inimigoVelCarro);
				inimigos.push_back(ci);
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
			childElem->Attribute("x", &linha->vEsqSup.x);
			childElem->Attribute("y", &linha->vEsqSup.y);
			childElem->Attribute("width", &linha->largura);
			childElem->Attribute("height", &linha->altura);
			linha->fill = converterCor(childElem->Attribute("fill"));
		}
	}

	relativizarPontos(jogador, inimigos, pistaInterna, pistaExterna, linha);

	free(arquivo);
}

void readXML(char* path, const char* file, EnemyAttr* enemyAttr, CarroJogador* jogador, list<CarroInimigo*>& inimigos, Circulo* pistaInterna, Circulo* pistaExterna, Retangulo* linha) {
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
	TiXmlElement* attrCarroInimigo = node->FirstChildElement("carroInimigo"); //Recupera o nó do carro inimigo

	//Atributos dos carros inimigos
	enemyAttr = (EnemyAttr*) malloc(sizeof(EnemyAttr));
	attrCarroInimigo->Attribute("freqTiro", &enemyAttr->inimigoFireRate);
	attrCarroInimigo->Attribute("velTiro", &enemyAttr->inimigoVelTiro);
	attrCarroInimigo->Attribute("velCarro", &enemyAttr->inimigoVelCarro);

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
	readSVG(caminho, arq, enemyAttr, jogador, inimigos, pistaInterna, pistaExterna, linha);

	//Atributos do carro
	jogador->setVelTiro(atof(attrCarro->Attribute("velTiro")));
	jogador->setVelCarro(atof(attrCarro->Attribute("velCarro")));

	free(arq);
	free(arquivo);

}