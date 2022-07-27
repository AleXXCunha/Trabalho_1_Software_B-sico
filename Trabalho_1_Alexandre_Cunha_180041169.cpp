/*
Trabalho 1 Software Básico - Montador
Aluno: Alexandre Abrahami Pinto da Cunha
Matrícula: 18/0041169
Sistema Operacional: Windows 11
Compilador: GCC/G++ versão 6.3.0 (MinGW)
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

//Nova variável que possui rótulo do token associado e seu valor de memória
struct dados{
	string rotulo;
	int memoria;
};

//Nova variável para a realização de operações de dados com valores de identificação
struct operacoes{
	string rotulo;
	int memoria;
	int valor;
};

//Função que recebe uma tabela com variáveis do tipo operacoes e a preenche, retornando sua nova versão
//É utilizada para atualizar a tabela de sinônimos (EQU)
auto atualizador_de_tabelas_de_operacoes(auto tabela, string rotulo, int memoria, int valor){
	operacoes operacoes_novo;

	operacoes_novo.rotulo = rotulo;
	operacoes_novo.memoria = memoria;
	operacoes_novo.valor = valor;
	

	tabela.push_back(operacoes_novo);

	return tabela;
}

//Função que recebe uma tabela com variáveis do tipo dados e a preenche, retornando sua nova versão
//É utilizada para atualizar a tabela de símbolos
auto atualizador_de_tabelas_de_dados(auto tabela, string rotulo, int memoria){
	dados dados_novo;

	dados_novo.rotulo = rotulo;
	dados_novo.memoria = memoria;

	tabela.push_back(dados_novo);

	return tabela;
}

//Função que pega uma linha e a formata, apagando todos os comentários e deixando todos os seus carateres maiúsculos
//Em seguida, ela pega cada token de uma linha e os coloca em um vetor para facilitar análise.
auto pega_tokens(string linha){
	vector<string> tokens; 
	string token, linha_nova;
	int i;

	for (i = 0; i < linha.length(); i++){
	 	linha[i] = toupper(linha[i]);
	}

	stringstream apagar_comentarios(linha);
	getline(apagar_comentarios, linha_nova, ';');

	stringstream pegaToken(linha_nova);
	while(pegaToken >> token){
		tokens.push_back(token);
	}

	return tokens;
}

//Possibilita criar um rótulo, dar quebra de linha e continuar a linha depois (o rótulo é equivalente a linha seguinte)
auto quebra_linha(ifstream &arquivo, auto tokens, int &contador_de_linhas){
	vector<string> rotulo_quebra_linha;
	string linha;

	while(rotulo_quebra_linha.empty()) {

		if (arquivo.eof())
			break;

		if (tokens.size() < 2){
			getline(arquivo, linha);
			contador_de_linhas++;
			rotulo_quebra_linha = pega_tokens(linha);
			tokens.insert(tokens.end(), rotulo_quebra_linha.begin(), rotulo_quebra_linha.end());
		} else {
			break;
		}
	}

	return tokens;

}

//Função que gera cada linha do arquivo de saída
void gerar_saida(ofstream &arquivosaida, auto tokens){
	int i;

	for (i=0; i < tokens.size(); i++){
			arquivosaida << tokens[i] << ' ';
		}
		arquivosaida << "\n";
}

//Verifica se todos os tokens possuem caracteres válidos (considerando casos especiais como CONST e COPY) e tamanho válido
void analisador_lexico(auto tokens, int contador_de_linhas){
	int i, j, linha_com_rotulo = 0;

	for (i=0; i < tokens.size(); i++){
		if (tokens[i].find(':') != string::npos){
			tokens[i] = tokens[i].substr(0, tokens[i].length() - 1);
			linha_com_rotulo++;
		}

		if (tokens[i].length() > 99)
			cout << "Erro lexico: token com mais de 99 caracteres. Linha: " <<  contador_de_linhas << endl;

		for (j=0; j < tokens[i].length(); j++){
			if (tokens[i][j] == ','){
				if ((tokens[linha_com_rotulo] != "COPY") || (j != tokens[i].length() - 1) || (i != (linha_com_rotulo + 1)))
					cout << "Erro lexico: token invalido. Linha: " <<  contador_de_linhas << endl;
			} else if (tokens[i][j] == '-'){
				if (tokens.size() > 1){
					if ((tokens[linha_com_rotulo] != "CONST") || (j != 0) || (i != (linha_com_rotulo + 1)))
						cout << "Erro lexico: token invalido. Linha: " <<  contador_de_linhas << endl;
				} else {
					cout << "Erro lexico: token invalido. Linha: " <<  contador_de_linhas << endl;
				}
			} else if (!isalnum(tokens[i][j]) && (tokens[i][j] != '_')) 
				cout << "Erro lexico: token invalido. Linha: " <<  contador_de_linhas << endl;
		}
		
		//Verifica se o primeiro caractere é número
		if (isdigit(tokens[i][0])){
			if (tokens.size() > 1){
				if ((tokens[linha_com_rotulo] != "CONST") || (i != (linha_com_rotulo + 1)))
					cout << "Erro lexico: token invalido. Linha: " <<  contador_de_linhas << endl;
			} else {
				cout << "Erro lexico: token invalido. Linha: " <<  contador_de_linhas << endl;
			}
		}
	}
}

//Função que auxilia o analisador sintático, serve para analisar intruções e seus operandos
void analise_instrucao(auto tokens, int posicao, auto tabela_de_instrucoes, auto tabela_de_diretivas, int contador_de_linhas, bool &token_valido){
	int i, j, k;

	for(i=0; i < 14; i++){
		if (tokens[posicao] == tabela_de_instrucoes[i].rotulo){
			token_valido = true;
			if((tokens.size() - (posicao + 1)) != tabela_de_instrucoes[i].valor){
				cout << "Erro sintatico: instrucao com a quantidade de operandos errado. Linha: " << contador_de_linhas << endl;

			//Analisando o caso especial de COPY
			} else if (i==8){

				//Verifica se os operandos de COPY são separados por vírgula
				if(tokens[(posicao + 1)][tokens[(posicao + 1)].length() - 1] != ',') {
					cout << "Erro sintatico: token invalido, operandos de COPY nao separados por virgula. Linha: " << contador_de_linhas << endl;

				//Verifica se nenhum dos operandos de COPY possui um nome de instrução ou de diretiva
				} else {
					tokens[(posicao + 1)] = tokens[(posicao + 1)].substr(0, tokens[(posicao + 1)].length() - 1);
					for (j=0; j < 14; j++){
						if ((tokens[(posicao + 1)] == tabela_de_instrucoes[j].rotulo) || (tokens[(posicao + 2)] == tabela_de_instrucoes[j].rotulo))
							cout << "Erro sintatico: token invalido, operandos com nome de instrucao. Linha: " << contador_de_linhas << endl;
					}

					for (j=0; j < 2; j++){
						if ((tokens[(posicao + 1)] == tabela_de_diretivas[j].rotulo) || (tokens[(posicao + 2)] == tabela_de_diretivas[j].rotulo))
							cout << "Erro sintatico: token invalido, operandos com nome de diretiva. Linha: " << contador_de_linhas << endl;
					}
				}

			//Verifica se nenhum dos operandos das outras instruções possui um nome de instrução ou de diretiva
			} else {
				for (j=0; j < 14; j++){
					for (k = (posicao + 1); k < tokens.size(); k++){
						if (tokens[k] == tabela_de_instrucoes[j].rotulo)
							cout << "Erro sintatico: token invalido, operandos com nome de instrucao. Linha: " << contador_de_linhas << endl;
					}
				}

				for (j=0; j < 2; j++){
					for (k = (posicao + 1); k < tokens.size(); k++){
						if (tokens[k] == tabela_de_diretivas[j].rotulo)
							cout << "Erro sintatico: token invalido, operandos com nome de diretiva. Linha: " << contador_de_linhas << endl;
					}
				}
			}
		}
	}
}

//Verifica se, em um dado contexto de uma linha, os tokens são permitidos/validos e sua quantidade está correta
void analisador_sintatico(ifstream &arquivo, auto tokens, auto tabela_de_instrucoes, auto tabela_de_diretivas, int &contador_de_linhas){
	int i;
	bool token0_valido = false, token1_valido = false;

	//Verifica se o token analisado é rótulo
	if (tokens[0].find(':') != string::npos){
		tokens[0].erase(tokens[0].end()-1);
		token0_valido = true;

		//Verifica se não existe mais de um rótulo na mesma linha
		if (tokens[0].find(':') != string::npos){
			cout << "Erro sintatico: mais de um rotulo na mesma linha. Linha: " << contador_de_linhas << endl;
		} else{
			for (i=1; i < tokens.size(); i++){
				if (tokens[i].find(':') != string::npos)
					cout << "Erro sintatico: mais de um rotulo na mesma linha. Linha: " << contador_de_linhas << endl;
			}
		}

		if (tokens.size() > 1){

			//Verifica se o rótulo está associado à uma diretiva
			for(i=0; i < 2; i++){
				if (tokens[1] == tabela_de_diretivas[i].rotulo){
					token1_valido = true;
					if((tokens.size() - 2) != tabela_de_diretivas[i].memoria){
						cout << "Erro sintatico: instrucao com a quantidade de operandos errado. Linha: " << contador_de_linhas << endl;
					} else if(tokens[1] == "CONST"){
						if(tokens[2].find_first_not_of("-0123456789") != string::npos)
						cout << "Erro sintatico: token invalido, CONST nao seguido por numero inteiro. Linha: " << contador_de_linhas << endl;
					}
				}
			}

			//Verifica se o rótulo está associado à uma instrução
			analise_instrucao(tokens, 1, tabela_de_instrucoes, tabela_de_diretivas, contador_de_linhas, token1_valido);

		} else {
			cout << "Erro sintatico: instrucao com a quantidade de operandos errado. Linha: " << contador_de_linhas << endl;
		}

		if (!token1_valido)
		cout << "Erro sintatico: token invalido, operacao com nome errado. Linha: " << contador_de_linhas << endl;

	}

	//Verifica se o token analisado é instrução
	analise_instrucao(tokens, 0, tabela_de_instrucoes, tabela_de_diretivas, contador_de_linhas, token0_valido);

	if (!token0_valido)
		cout << "Erro sintatico: token invalido, operacao com nome errado. Linha: " << contador_de_linhas << endl;
}

//O pré-processamento de diretivas analisa se existe a diretiva EQU, substituindo o símbolo associado a ela por seus respectivo valor em todo o código.
//Ele também analisa se existe a diretiva IF, instruindo o montador a incluir a linha seguinte do código ou não a depender do rótulo
void pre_processamento_diretivas(ifstream &arquivo, ofstream &arquivosaida){
	string linha;
	vector<string> tokens;
	vector<operacoes> tabela_de_sinonimos;
	int i,j, contador_de_linhas = 0;
	bool rotulo_declarado = false, equ_utilizado = false;

	while(!arquivo.eof()){
		getline(arquivo, linha);
		contador_de_linhas++;
		tokens = pega_tokens(linha);

		if (!tokens.empty()){

			//Verifica se o token analisado é rótulo
			if (tokens[0].find(':') != string::npos){
				tokens[0].erase(tokens[0].end()-1);

				tokens = quebra_linha(arquivo, tokens, contador_de_linhas);
				if (tokens.size() > 1){
					//Verifica se o rótulo está associado à diretiva EQU
					if (tokens[1] == "EQU"){
						tabela_de_sinonimos = atualizador_de_tabelas_de_operacoes(tabela_de_sinonimos, tokens[0], stoi(tokens[2]), contador_de_linhas);
					//Caso ele não esteja, verifica se os tokens seguintes à este rótulo possui algum sinônimo para ser substituído
					} else {
						for (i=0; i < tabela_de_sinonimos.size(); i++){
							for (j=1; j < tokens.size(); j++){
								if (tabela_de_sinonimos[i].rotulo == tokens[j]){
									tokens[j] = to_string(tabela_de_sinonimos[i].memoria);
									tabela_de_sinonimos[i].valor = 0;
								}
							}
						}
						tokens[0] += ":";
						gerar_saida(arquivosaida, tokens);
					}
				} else {
					tokens[0] += ":";
					gerar_saida(arquivosaida, tokens);
				}

			//Verifica se o token é um IF
			} else if (tokens[0] == "IF"){
				for (i=0; i < tabela_de_sinonimos.size(); i++){
					if (tokens.size() > 1){
						if (tabela_de_sinonimos[i].rotulo == tokens[1]){
							tabela_de_sinonimos[i].valor = 0;
							//Pula a próxima linha caso a expressão durante o tempo de montagem seja igual a zero
							if(!tabela_de_sinonimos[i].memoria){
								getline(arquivo, linha);
								contador_de_linhas++;
							}
							rotulo_declarado = true;
						}
					}
				}

				//Verifica se o rótulo posterior ao IF foi declarado por um EQU
				if (!rotulo_declarado){
					cout << "Erro semantico: IF com rotulo nao declarado por um EQU. Linha: " << contador_de_linhas << endl;
				}

			//Verifica os tokens desta linha possuem algum sinônimo para ser substituído
			} else {
				for (i=0; i < tabela_de_sinonimos.size(); i++){
					for (j=0; j < tokens.size(); j++){
						if (tabela_de_sinonimos[i].rotulo == tokens[j]){
							tokens[j] = to_string(tabela_de_sinonimos[i].memoria);
							tabela_de_sinonimos[i].valor = 0;
						}
					}
				}
				gerar_saida(arquivosaida, tokens);
			}
		}
	}

	//Verifica o token é um EQU com rótulo não utilizado
	for (i=0; i < tabela_de_sinonimos.size(); i++){
		if (tabela_de_sinonimos[i].valor != 0)
			cout << "Erro semantico: EQU com rotulo nao utilizado. Linha: " << tabela_de_sinonimos[i].valor << endl;
	}
}

//O pré-processamento de macros analisa se existe uma e única MACRO, guardando o nome do seu rótulo e copiando o seu código até encontrar o ENDMACRO.
//Toda vez que o rótulo da MACRO for encontrada, ele substituirá o seu rótulo pelo código copiado.
void pre_processamento_macros(ifstream &arquivo, ofstream &arquivosaida){
	string linha, rotulo_macro;
	vector<string> tokens;
	vector<vector<string>> linhas_macro; 
	int contador_de_linhas = 0, linha_da_macro, i, j;
	bool analisando_macro = false, macro_utilizada = true;

	while(!arquivo.eof()){
		getline(arquivo, linha);
		contador_de_linhas++;
		tokens = pega_tokens(linha);

		if (!tokens.empty()){

			//Verifica se o token é um ENDMACRO, caso seja deve-se parar de copiar o código da macro
			if (tokens[0] == "ENDMACRO"){
				analisando_macro = false;
			} else if (analisando_macro){
				linhas_macro.push_back(tokens);
			}

			//Verifica se o token analisado é rótulo
			if (tokens[0].find(':') != string::npos){

				tokens = quebra_linha(arquivo, tokens, contador_de_linhas);
				if (tokens.size() > 1){
					//Verifica se o rótulo é o nome de um MACRO, caso seja começa a copiar o código da macro e define que essa macro não foi utilizada ainda
					if (tokens[1] == "MACRO"){
						tokens[0].erase(tokens[0].end()-1);
						analisando_macro = true;
						macro_utilizada = false;
						rotulo_macro = tokens[0];
						linha_da_macro = contador_de_linhas;
					} else {
						if (!analisando_macro)
							gerar_saida(arquivosaida, tokens);
					}
				} else {
					if (!analisando_macro)
						gerar_saida(arquivosaida, tokens);
				}

			//Verifica se o token é o rótulo da MACRO, caso seja copia o seu código em seu lugar
			} else if (tokens[0] == rotulo_macro){
				macro_utilizada = true;
				for (i=0; i < linhas_macro.size(); i++){
					gerar_saida(arquivosaida, linhas_macro[i]);
				}

			//Verifica se é diferente de ENDMACRO para evitar colocar o token no arquivo de saída
			} else if (tokens[0] != "ENDMACRO") {
				if (!analisando_macro)
					gerar_saida(arquivosaida, tokens);
			}
		}

	}

	//Verificação de erros
	if (analisando_macro){
		cout << "Erro semantico: Falta de ENDMACRO. Linha: " << linha_da_macro << endl;
	}
	if (!macro_utilizada){
		cout << "Erro semantico: MACRO nao utilizada. Linha: " << linha_da_macro << endl;
	}
}

//A primeira passagem recebe uma tabela de símbolos vazia e a preenche com os rótulos que forem sendo encontrados
//Os erros léxicos e sintáticos não são analisados aqui, ela somente se preocupa em não repetir rótulos na tabela (erro semântico)
auto primeira_passagem(ifstream &arquivo, auto tabela_de_simbolos){
	string linha;
	vector<string> tokens;
	int i, contador_de_linhas = 0, contador_de_posicoes = 0;
	bool existe, fim_section_text = false, section_data = false, data;

	while(!arquivo.eof()){
		getline(arquivo, linha);
		contador_de_linhas++;
		tokens = pega_tokens(linha);
		existe = false;
		data = false;

		if (!tokens.empty()){

			//Verifica se o token analisado é rótulo
			if (tokens[0].find(':') != string::npos){
				tokens[0].erase(tokens[0].end()-1);

				//Verifica se o rótulo já não existe na tabela de símbolos 
				for (i=0; i < tabela_de_simbolos.size(); i++){
					if (tabela_de_simbolos[i].rotulo == tokens[0]){
						cout << "Erro semantico: Rotulo repetido. Linha: " << contador_de_linhas << endl;
						existe = true;
					}
				}

				if (tokens.size() > 1){
					if ((tokens[1] == "CONST" || tokens[1] == "SPACE") && !section_data)
						data = true;
				}

				//Se o rótulo não existir, atualiza na tabela
				if ((!existe) && (!data)){
					tabela_de_simbolos = atualizador_de_tabelas_de_dados(tabela_de_simbolos, tokens[0], contador_de_posicoes);
				} else if ((!existe) && (data)) {
					tabela_de_simbolos = atualizador_de_tabelas_de_dados(tabela_de_simbolos, tokens[0], -1);
				}

				tokens = quebra_linha(arquivo, tokens, contador_de_linhas);

				if (tokens.size() > 1){
					if (tokens[1] == "COPY"){
						contador_de_posicoes = contador_de_posicoes + 3;
					} else if (tokens[1] == "STOP") {
						contador_de_posicoes++;
						section_data = true;
					} else if (tokens[1] == "CONST" || tokens[1] == "SPACE"){
						if (!data)
							contador_de_posicoes++;
					} else {
						contador_de_posicoes = contador_de_posicoes + 2;
					}
				}

			} else if (tokens[0] == "COPY"){
				contador_de_posicoes = contador_de_posicoes + 3;
			} else if (tokens[0] == "STOP"){
				contador_de_posicoes++;
				section_data = true;

				for (i=0; i < tabela_de_simbolos.size(); i++){
					if (tabela_de_simbolos[i].memoria == -1){
						tabela_de_simbolos[i].memoria = contador_de_posicoes;
						contador_de_posicoes++;
					}
				}

			} else {
				contador_de_posicoes = contador_de_posicoes + 2;
			}
		}
	}

	
	return tabela_de_simbolos;
}

//A segunda passagem recebe a tabela de símbolos preenchida pela primeira passagem e gera o código objeto com base nos valores de mémorias, opcodes, espaços reservados e constantes
//Os erros léxicos e sintáticos são analisados aqui pelas suas respectivas funções analisadoras, além da identificação de tokens com rótulos não declarados (erro semântico)
void segunda_passagem(ifstream &arquivo, ofstream &arquivosaida, auto tabela_de_simbolos){
	string linha;
	vector<string> tokens, section_data;
	operacoes tabela_de_instrucoes[] = {{"ADD", 1, 1},{"SUB", 2, 1},{"MULT", 3, 1},{"DIV", 4, 1},{"JMP", 5, 1},{"JMPN", 6, 1},{"JMPP", 7, 1},{"JMPZ", 8, 1},{"COPY", 9, 2},{"LOAD", 10, 1},{"STORE", 11, 1},{"INPUT", 12, 1},{"OUTPUT", 13, 1},{"STOP", 14, 0}};
	dados tabela_de_diretivas[] = {{"CONST", 1},{"SPACE", 0}};
	int contador_de_linhas = 0, i, j;

	while(!arquivo.eof()){

		getline(arquivo, linha);
		contador_de_linhas++;
		tokens = pega_tokens(linha);
		int simbolo = 0, instrucao = 0, diretiva = 0;

		if (!tokens.empty()){

			///Verifica se o token analisado é rótulo, e ajeitando a quebra de linha se ela existir
			if (tokens[0].find(':') != string::npos)
				tokens = quebra_linha(arquivo, tokens, contador_de_linhas);
		
			//Chama os analisadores léxicos e sintáticos para verificar os erros
			analisador_lexico(tokens, contador_de_linhas);
			analisador_sintatico(arquivo, tokens, tabela_de_instrucoes, tabela_de_diretivas, contador_de_linhas);

			//Coloca todos os tokens no arquivo de saída
			for(i=0; i < tokens.size(); i++){

				//Verifica se é instrução
				for(j=0; j < 14; j++){
					if (tokens[i] == tabela_de_instrucoes[j].rotulo){
						if (i > 0)
							instrucao++;
						arquivosaida << tabela_de_instrucoes[j].memoria << ' ';
					}
				}

				//Verifica se é símbolo
				for(j=0; j < tabela_de_simbolos.size(); j++){
					if (tokens[i] == tabela_de_simbolos[j].rotulo || tokens[i] == tabela_de_simbolos[j].rotulo + ","){
						if (i > 0)
							simbolo++;
						arquivosaida << tabela_de_simbolos[j].memoria << ' ';
					}
				}

				//Verifica se é diretiva
				if (tokens[i] == "CONST"){
					if (i > 0)
						diretiva++;
					if (tokens.size() > (i+1)){
						diretiva++;
						section_data.push_back(tokens[i+1]);
					}
				}
				if (tokens[i] == "SPACE"){
					if (i > 0)
						diretiva++;
					section_data.push_back("0");
				}

			}

			//Se a quantidade de tokens for diferente da quantidade de instruções, diretivas e simbolos somados na linha, algum token é desconhecido
			if (((instrucao + diretiva + simbolo) != (tokens.size() - 1)) && (tokens.size() > 1))
				cout << "Erro semantico: Declaracao de rotulo ausente. Linha: " << contador_de_linhas << endl;
			
		}
	}

	for (i=0; i < section_data.size(); i++){
		arquivosaida << section_data[i] << ' ';
	}
}

int main(int argc, char* argv[]) {
	string funcionamento(argv[1]), arquivo_entrada(argv[2]), arquivo_saida(argv[3]);
	vector<dados> tabela_de_simbolos;

	//Abre o arquivo de entrada para leitura
	ifstream ler_arquivo_entrada(arquivo_entrada);
	if(!ler_arquivo_entrada)
		std::cout << "Ocorreu um erro na abertura do arquivo" << std::endl;
	
	//Cria um arquivo de saída para escrita 
	ofstream gerar_arquivo_saida(arquivo_saida);
	if(!gerar_arquivo_saida)
		std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;

	//Verifica como vai ser o funcionamento do montador
	if (funcionamento == "-p"){
		pre_processamento_diretivas(ler_arquivo_entrada, gerar_arquivo_saida);
	} else if (funcionamento == "-m"){
		pre_processamento_macros(ler_arquivo_entrada, gerar_arquivo_saida);
	} else if (funcionamento == "-o"){

		//Cria um arquivo intermediário para escrita do pré-processamento de diretivas
		ofstream gerar_arquivo_pre_processado_diretiva("arquivo_diretiva.asm");
		if(!gerar_arquivo_pre_processado_diretiva)
			std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;

		pre_processamento_diretivas(ler_arquivo_entrada, gerar_arquivo_pre_processado_diretiva);

		gerar_arquivo_pre_processado_diretiva.close();

		//Cria um arquivo intermediário para escrita do pré-processamento de macros e abre o arquivo do pré-processamento das diretivas para leitura
		ifstream ler_arquivo_pre_processado_diretiva("arquivo_diretiva.asm");
		if(!ler_arquivo_pre_processado_diretiva)
			std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;
		ofstream gerar_arquivo_pre_processado_macro("arquivo_macro.asm");
		if(!gerar_arquivo_pre_processado_macro)
			std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;

		pre_processamento_macros(ler_arquivo_pre_processado_diretiva, gerar_arquivo_pre_processado_macro);

		gerar_arquivo_pre_processado_macro.close();

		//Abre o arquivo do pré-processamento de macros para leitura
		ifstream ler_arquivo_pre_processado_macro("arquivo_macro.asm");
		if(!ler_arquivo_pre_processado_macro)
			std::cout << "Ocorreu um erro na criacao do arquivo de saida" << std::endl;
		

		tabela_de_simbolos = primeira_passagem(ler_arquivo_pre_processado_macro, tabela_de_simbolos);
		ler_arquivo_pre_processado_macro.clear();
		ler_arquivo_pre_processado_macro.seekg(0);
		segunda_passagem(ler_arquivo_pre_processado_macro, gerar_arquivo_saida, tabela_de_simbolos);

		ler_arquivo_pre_processado_macro.close();

	} else {
		cout << "Erro: Comando nao reconhecido" << endl;
	}

	//Apaga os arquivos intermediários utilizados nos pré-processamentos se existirem
	remove("arquivo_diretiva.asm");
	remove("arquivo_macro.asm");
	ler_arquivo_entrada.close();
	gerar_arquivo_saida.close();

	return 0;
}