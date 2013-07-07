#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define DEBUG_1

#define UHET_TECLA_ESC 27

#define FALSE 0
#define TRUE 1

#define LARGURA 800
#define ALTURA 600
#define X_INI 0
#define X_FIM 1
#define Y_INI 2
#define Y_FIM 3
#define X_CENTRO 4
#define Y_CENTRO 5

#define AMARELO "\033[01;33m"
#define VERMELHO "\033[01;31m"
#define VERDE "\033[01;32m"
#define AZUL "\033[01;34m"
#define MAGENTA "\033[01;35m"
#define BRANCO "\033[01;37m"
#define PRETO "\033[01;30m"
#define NEUTRO "\033[00m"

#define CAINDO 0
#define SUBINDO 1
#define DIREITA 10
#define ESQUERDA 11

#define AMAX 90
#define AMIN 10

#define MAX 50
//#define ORTO 1

//==== classes
struct uhet_obj_string{
	char* str;
};

struct uhet_obj_cor{

	float r;
	float g;
	float b;
	float a;
};

struct uhet_obj_coordenadas{

	float x;
	float y;
	float z;
};

struct uhet_obj_box{

	struct uhet_obj_coordenadas* posicao;
	struct uhet_obj_coordenadas* escala;
//	struct uhet_obj_coordenadas* translate;
	struct uhet_obj_cor* cor_borda;
	struct uhet_obj_cor* cor_miolo;
	int tamanho;
	int preenchido;
	int ativo;

};

struct uhet_obj_esfera{

	struct uhet_obj_coordenadas* posicao;
	struct uhet_obj_coordenadas* escala;
	struct uhet_obj_cor* cor_borda;
	struct uhet_obj_cor* cor_miolo;
	float raio;
	int preenchido;

};

struct uhet_obj_jogo{

	int largura;
	int altura;
	int pontos;
	int vidas;
	struct uhet_obj_string* nome;
	int x;
	int y;
	int fullscreen;
	int bola_angulo;
	int bola_sentido;
	int bola_direcao;
	float bola_aceleracao;
	float bola_velocidade;
	int n_blocos;
	int pause;
};

struct uhet_obj_camera{
	
	GLfloat x;
	GLfloat y;
	GLfloat z;
	float zoom;
};

//==== declaracao de funcoes
static void uhet_ctrl_reshape (int largura, int altura);
float uhet_obj_box_get(struct uhet_obj_box* box, int opt);
void uhet_show_placar();
void uhet_show_game_placar();
void uhet_show_game_fim();
void uhet_show_game_help();
void uhet_show_msg(char* str){

	fprintf(stdout,"%sUHETBALL:%s %s%s%s", AZUL, NEUTRO, VERDE, str, NEUTRO);
}

//==== objetos globais
struct uhet_obj_box* g_jogador = NULL;
struct uhet_obj_jogo* g_jogo = NULL;
struct uhet_obj_camera* g_camera = NULL;
struct uhet_obj_box* g_borda = NULL;
struct uhet_obj_box* g_placar = NULL;
struct uhet_obj_box* g_bloco_1 = NULL;
struct uhet_obj_box* g_bola = NULL;
float x,y;
struct uhet_obj_box* ga_blocos[MAX];
float ORTO;


//==== uhet_obj_cor_string
struct uhet_obj_string* uhet_obj_string_constructor(char* str){

	struct uhet_obj_string* string = (struct uhet_obj_string*) malloc(sizeof(struct uhet_obj_string));
	char* s = (char*) malloc(sizeof(char[strlen(str)+1]));
	strcpy(s,str);
	string->str = s;
	return string;
}

//==== uhet_obj_camera

void uhet_obj_camera_set(struct uhet_obj_camera* camera){
	
	glRotatef(camera->x, 1.0, 0.0, 0.0);
	glRotatef(camera->y, 0.0, 1.0, 0.0);
//	glRotatef(camera->z, 0.0, 0.0, 1.0);
}

struct uhet_obj_camera* uhet_obj_camera_constructor(){
	
	struct uhet_obj_camera* camera = (struct uhet_obj_camera*) malloc(sizeof(struct uhet_obj_camera));
	camera->x = 0;
	camera->y = 0;
	camera->z = 0;
	uhet_obj_camera_set(camera);
	camera->zoom = 1.0;
	return camera;
}

void uhet_obj_camera_zoom(struct uhet_obj_camera* camera){

	glScalef(camera->zoom, camera->zoom, camera->zoom);
}

//==== uhet_obj_cor
struct uhet_obj_cor* uhet_obj_cor_contructor(){

	struct uhet_obj_cor* cor = (struct uhet_obj_cor*) malloc(sizeof(struct uhet_obj_cor));
	cor->r = 0;
	cor->g = 0;
	cor->b = 0;
	cor->a = 0;
	return cor;
}

void uhet_obj_cor_print(struct uhet_obj_cor* cor){

	uhet_show_msg("uhet_obj_cor: ");
	fprintf(stdout,"[\"%f\",\"%f\",\"%f\"]\n", cor->r, cor->g, cor->b);

}

void uhet_obj_cor_set(struct uhet_obj_cor* cor, float red, float green, float blue, float alfa){

	if (cor){
		cor->r = red;
		cor->g = green;
		cor->b = blue;
		cor->a = alfa;
	} else {
		uhet_show_msg("[erro] tentando setar cor nao alocada");
	}
}

//==== uhet_obj_coordenadas
struct uhet_obj_coordenadas* uhet_obj_coordenadas_contructor(){

	struct uhet_obj_coordenadas* pos = (struct uhet_obj_coordenadas*) malloc(sizeof(struct uhet_obj_coordenadas));
	pos->x = 0;
	pos->y = 0;
	pos->z = 0;
	return pos;
}

void uhet_obj_coordenadas_print(struct uhet_obj_coordenadas* pos){

	uhet_show_msg("uhet_obj_coordenadas: ");
	fprintf(stdout,"[\"%f\",\"%f\",\"%f\"]\n", pos->x, pos->y, pos->z);
}

void uhet_obj_coordenadas_set(struct uhet_obj_coordenadas* pos, float x, float y, float z){

	if (pos){
		pos->x = x;
		pos->y = y;
		pos->z = z;
	} else {
		uhet_show_msg("[erro] tentando setar coordenadas nao alocada");
	}
}

//==== uhet_obj_jogo

struct uhet_obj_jogo* uhet_obj_jogo_constructor(){

	struct uhet_obj_jogo* jogo = (struct uhet_obj_jogo*) malloc(sizeof(struct uhet_obj_jogo));
	jogo->largura = 800;
	jogo->altura = 600;
	jogo->pontos = 0;
	jogo->vidas = 3;
	jogo->nome = uhet_obj_string_constructor("uhetball");
	jogo->x = 0;
	jogo->y = 0;
	jogo->fullscreen = FALSE;
	jogo->bola_sentido = CAINDO; //subindo, caindo
	jogo->bola_direcao = FALSE; //direita, esquerda, false
	jogo->bola_velocidade = 0.1;
	jogo->bola_aceleracao = 0;
	jogo->n_blocos = 0;
	jogo->pause = TRUE;
	return jogo;
}

void uhet_obj_jogo_print(struct uhet_obj_jogo* jogo){

	uhet_show_msg("uhet_obj_jogo:\n");
	fprintf(stdout,"\tjanela: %i x %i: \n",jogo->largura, jogo->altura);
	fprintf(stdout,"\tpontos: %i: \n",jogo->pontos);

}

//==== uhet_obj_array

void uhet_obj_array_contructor(){

	int i;
	for (i = 0 ; i < MAX; i++){

		ga_blocos[i] = NULL;

	}

}

//==== uhet_obj_esfera
struct uhet_obj_esfera* uhet_obj_esfera_contructor(){

	struct uhet_obj_esfera* e = (struct uhet_obj_esfera*) malloc(sizeof(struct uhet_obj_esfera));
	e->cor_borda = uhet_obj_cor_contructor();
	e->cor_miolo = uhet_obj_cor_contructor();
	e->posicao = uhet_obj_coordenadas_contructor();
	e->escala = uhet_obj_coordenadas_contructor();
	e->raio = 0;
	e->preenchido = TRUE;
	return e;
}


//==== uhet_obj_box
struct uhet_obj_box* uhet_obj_box_contructor(){

	struct uhet_obj_box* jogador = (struct uhet_obj_box*) malloc(sizeof(struct uhet_obj_box));
	jogador->cor_borda = uhet_obj_cor_contructor();
	jogador->cor_miolo = uhet_obj_cor_contructor();
	jogador->posicao = uhet_obj_coordenadas_contructor();
	jogador->escala = uhet_obj_coordenadas_contructor();
//	jogador->translate = uhet_obj_coordenadas_contructor();
	jogador->tamanho = 0;
	jogador->preenchido = FALSE;
	jogador->ativo = TRUE;
	return jogador;
}

void uhet_obj_box_print(struct uhet_obj_box* jogador){

//	uhet_show_msg("uhet_obj_box:\n");
//	uhet_obj_cor_print(jogador->cor_miolo);
//	uhet_obj_cor_print(jogador->cor_borda);
	
	uhet_show_msg("uhet_obj_box: ");
	fprintf(stdout,"[%.3f, ", uhet_obj_box_get(jogador,X_INI));
	fprintf(stdout,"%.3f, ", uhet_obj_box_get(jogador,X_FIM));
	fprintf(stdout,"%.3f, ", uhet_obj_box_get(jogador,Y_INI));
	fprintf(stdout,"%.3f] ", uhet_obj_box_get(jogador,Y_FIM));
	fprintf(stdout,"%s%s%s [%.3f, %.3f]\n", VERDE, "centro:", NEUTRO, uhet_obj_box_get(jogador,X_CENTRO), uhet_obj_box_get(jogador,Y_CENTRO));
		
	
//	uhet_obj_coordenadas_print(jogador->posicao);
//	uhet_obj_coordenadas_print(jogador->escala);
//	uhet_show_msg("tamanho: "); fprintf(stdout,"%i\n", jogador->tamanho);
}

float uhet_obj_box_get(struct uhet_obj_box* box, int opt){

	float r = 0;
	switch(opt){
		case X_INI: r = (box->posicao->x * box->escala->x) - (box->escala->x / 2); break;
		case X_FIM:
//				uhet_show_msg("X_FIM\n");
				r = (box->posicao->x * box->escala->x) + (box->escala->x / 2);
				break;
		case Y_INI:
//				uhet_show_msg("Y_INI\n");
				r = (box->posicao->y * box->escala->y) + (box->escala->y / 2);
				break;
		case Y_FIM:
//				uhet_show_msg("Y_FIM\n");
				r = (box->posicao->y * box->escala->y) - (box->escala->y / 2);
				break;
		case X_CENTRO: r = box->posicao->x * box->escala->x; break;
		case Y_CENTRO: r = box->posicao->y * box->escala->y; break;
	}
	return r;
}


//==== controle
static void uhet_ctrl_teclado(unsigned char tecla, int x, int y){

	switch(tecla){
		case UHET_TECLA_ESC:
//				uhet_show_msg("tecla pressionada: UHET_TECLA_ESC\n");
				g_jogo->n_blocos = 0;
//				glutLeaveMainLoop();
				break;
		case '+': 
//				uhet_show_msg("tecla pressionada: +\n");
				g_camera->zoom += 0.02;
//				ORTO += 0.1;
				break;
		case '-': 
//				uhet_show_msg("tecla pressionada: -\n");
				g_camera->zoom -= 0.02;
//				ORTO -= 0.1;
				break;
		case 'n':
				g_camera->zoom = 1.0;
				break;
		case 'f':
				if (!g_jogo->fullscreen){
					glutFullScreen();
					glutPositionWindow(0,0);
					g_jogo->fullscreen = TRUE;
				} else {
//					g_jogo->x = rint(glutGet(GLUT_SCREEN_WIDTH)/2) - (LARGURA/2);
//					g_jogo->y = rint(glutGet(GLUT_SCREEN_HEIGHT)/2) - (ALTURA/2);
//					g_jogo->largura = LARGURA;
//					g_jogo->altura = ALTURA;

					glutReshapeWindow(g_jogo->largura,g_jogo->altura);
					glutPositionWindow(g_jogo->x-1,g_jogo->y-17); //gambiarra

					g_jogo->fullscreen = FALSE;
				}
				break;
		case '1': 
//				uhet_show_msg("tecla pressionada: 1\n");
				g_jogador->cor_miolo->r += 0.1;
				g_jogador->cor_miolo->g += 0.1;
				g_jogador->cor_miolo->b += 0.1;
				break;
		case '2': 
//				uhet_show_msg("tecla pressionada: 2\n");
				g_jogador->cor_miolo->r -= 0.1;
				g_jogador->cor_miolo->g -= 0.1;
				g_jogador->cor_miolo->b -= 0.1;
				break;
		case '3': g_jogo->bola_aceleracao += 0.1; break;
		case '4':
				g_jogo->bola_aceleracao -= 0.1;
				if (g_jogo->bola_aceleracao <= 0) g_jogo->bola_aceleracao = 0;
				
				break;
		case 'a': g_camera->y -= 10; break;
		case 'd': g_camera->y += 10; break;
		case 's': g_camera->x -= 10; break;
		case 'w': g_camera->x += 10; break;
		case 'j': 
//				uhet_show_msg("tecla pressionada: j\n");
				uhet_obj_box_print(g_jogador);
				break;
		case 'b': 
//				uhet_show_msg("tecla pressionada: j\n");
				uhet_obj_box_print(g_borda);
				break;
		case 'h':
				if (g_jogo->pause) g_jogo->pause = FALSE;
				else {
					g_jogo->pause = TRUE;
//					uhet_show_game_help();
				}

				break;
		case 'v': uhet_obj_box_print(g_bola); break;
		case 'p':
//				printf("x: %i y: %i | ", glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y));
//				printf("%i x %i\n", glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
				uhet_show_placar();
				break;

	}
	glutPostRedisplay(); //atualiza a tela
}

void uhet_ctrl_tecla_left(float val){

	g_jogador->posicao->x -= val;
	if (uhet_obj_box_get(g_jogador,X_INI) <= uhet_obj_box_get(g_borda,X_INI))
		g_jogador->posicao->x = (uhet_obj_box_get(g_borda,X_INI) + (g_jogador->escala->x/2))/ g_jogador->escala->x;
/*
xini = (px*ex) - (ex/2), logo:
px = ((xini + (ex/2))/ex
*/
}

void uhet_ctrl_tecla_right(float val){


	g_jogador->posicao->x += val;
	if ((uhet_obj_box_get(g_jogador,X_FIM)*1000) >= (uhet_obj_box_get(g_borda,X_FIM)*1000))
		g_jogador->posicao->x = (uhet_obj_box_get(g_borda,X_FIM) - (g_jogador->escala->x/2))/ g_jogador->escala->x;
/*
xfim = (px*ex) + (ex/2), logo:
px = (xfim - (ex/2)) / ex
*/
}

static void uhet_ctrl_especial(int tecla, int x, int y){

	switch(tecla){

		case GLUT_KEY_LEFT:  uhet_ctrl_tecla_left(0.25); break;
		case GLUT_KEY_RIGHT: uhet_ctrl_tecla_right(0.25); break;
		case GLUT_KEY_UP:    uhet_ctrl_tecla_right(1);	break;
		case GLUT_KEY_DOWN:  uhet_ctrl_tecla_left(1); break;
	}
	glutPostRedisplay(); //atualiza a tela
}


void uhet_desenha_box(struct uhet_obj_box* box){


	if (box->ativo){
		glPushMatrix();
			glScalef (box->escala->x, box->escala->y, box->escala->y);
			glTranslatef (box->posicao->x, box->posicao->y, box->posicao->z);
		
			if (box->preenchido) {
				glColor3f (box->cor_miolo->r,box->cor_miolo->g,box->cor_miolo->b);
				glutSolidCube (box->tamanho); //miolo
			}
		
			glColor3f (box->cor_borda->r,box->cor_borda->g,box->cor_borda->b);
			glutWireCube (box->tamanho);//borda
		glPopMatrix();
	}
}

static void uhet_ctrl_tela(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ORTO, ORTO, -ORTO, ORTO, -ORTO, ORTO);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
*/

	glPushMatrix();

//			uhet_obj_camera_zoom(g_camera);
		glPushMatrix();

			uhet_obj_camera_zoom(g_camera);

			uhet_obj_camera_set(g_camera);
			int i = 0;			
			while (ga_blocos[i]){
				uhet_desenha_box(ga_blocos[i]);

				i++;
			}
			uhet_desenha_box(g_bola);

			uhet_desenha_box(g_borda);
			uhet_desenha_box(g_jogador);
		glPopMatrix();
		
//		uhet_desenha_esfera();
		uhet_desenha_box(g_placar);
		uhet_show_game_placar();
		if (g_jogo->pause) uhet_show_game_help();
		if (g_jogo->n_blocos <=0) uhet_show_game_fim();
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}


static void uhet_ctrl_reshape (int largura, int altura){

	if (g_jogo->fullscreen == FALSE){
		g_jogo->x = glutGet(GLUT_WINDOW_X);
		g_jogo->y = glutGet(GLUT_WINDOW_Y);
		g_jogo->largura = largura;
		g_jogo->altura = altura;
	}
	
	glViewport(0, 0, largura, altura);
//	uhet_show_msg("redimensionando: ");
//	printf("x: %i y: %i | ", glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y));
//	printf("%i x %i\n", glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

static void uhet_movimento_bola(){


	switch(g_jogo->bola_sentido){

		case CAINDO:
			g_bola->posicao->y -= g_jogo->bola_velocidade + g_jogo->bola_aceleracao + y;
			break;
		case SUBINDO:
			g_bola->posicao->y += g_jogo->bola_velocidade + g_jogo->bola_aceleracao + y;
			break;
	}
	switch(g_jogo->bola_direcao){

		case DIREITA:
			g_bola->posicao->x += g_jogo->bola_velocidade + g_jogo->bola_aceleracao + x;
			break;
		case ESQUERDA:
			g_bola->posicao->x -= g_jogo->bola_velocidade + g_jogo->bola_aceleracao + x;
			break;
		case FALSE:
//			g_bola->posicao->x -= (g_jogo->bola_velocidade + g_jogo->bola_aceleracao) + x;
			g_bola->posicao->x = 0;
			break;

	}
}

void uhet_movimento_colisao_refletir(int porcentagem){

	if (g_jogo->bola_direcao == FALSE){
		if (porcentagem < 50)
			g_jogo->bola_direcao = ESQUERDA;
		else {
			g_jogo->bola_direcao = DIREITA;
		}
	}

	if (porcentagem > 50) porcentagem = 100 - porcentagem;

	x = 0;
	y = 0;
	if (porcentagem >= 0 && porcentagem < 10) { //70
		x = 0.5;
		y = 1.5;
	} else if (porcentagem >= 10 && porcentagem < 20){ //55
		x = 0.75;
		y = 1.25;
	} else if (porcentagem >= 20 && porcentagem < 30){ //45
		x = 1;
		y = 1;
	} else if (porcentagem >= 30 && porcentagem < 40){ //30
		x = 1.25;
		y = 0.75;
	} else if (porcentagem >= 40 && porcentagem <= 50){ //15
		x = 1.5;
		y = 0.5;
	}
//	uhet_show_msg("x,y: "); printf("%.3f x %.3f\n", x,y);
//	uhet_show_msg("porcentagem:: "); printf("%i\n", porcentagem);
}

void uhet_movimento_colisao_broco(){
	
	int i = 0;
	float pos_x_bola = uhet_obj_box_get(g_bola,X_CENTRO);
	float pos_y_bola = uhet_obj_box_get(g_bola,Y_CENTRO);
	while (ga_blocos[i]){

		if (ga_blocos[i]->ativo){
			float pos_xi_jogador = uhet_obj_box_get(ga_blocos[i],X_INI);
			float pos_xf_jogador = uhet_obj_box_get(ga_blocos[i],X_FIM);
			float pos_yi_jogador = uhet_obj_box_get(ga_blocos[i],Y_INI);
			float pos_yf_jogador = uhet_obj_box_get(ga_blocos[i],Y_FIM);

			//opa, bateu em mim!
			if ((pos_x_bola >= pos_xi_jogador) && ((pos_x_bola <= pos_xf_jogador))){
		
				if ((pos_y_bola <= pos_yi_jogador) && ((pos_y_bola >= pos_yf_jogador))){
			
					ga_blocos[i]->ativo = FALSE;
					uhet_show_placar();
					g_jogo->pontos++;
					g_jogo->n_blocos--;
//					if (g_jogo->n_blocos <= 0) glutLeaveMainLoop();
					system("aplay -q ./sound/exp.wav 2> /dev/null &");
//					uhet_show_msg("opa!\n");
					if (g_jogo->bola_sentido == SUBINDO) g_jogo->bola_sentido = CAINDO;
					else g_jogo->bola_sentido = SUBINDO;
				
//					if (g_jogo->bola_direcao == ESQUERDA) g_jogo->bola_direcao = DIREITA;
//					else if (g_jogo->bola_direcao == DIREITA) g_jogo->bola_direcao = ESQUERDA;

				}
			}
		}
		i++;
	}
}

void uhet_show_placar(){
	
	fprintf(stdout,"%s%s%s\n", AZUL, "_____________________", NEUTRO);
	fprintf(stdout,"%s%s%s", AZUL, "|", NEUTRO);
	uhet_show_msg("[PLACAR]  ");
	fprintf(stdout,"%s%s\n%s%s", AZUL,"|","|", NEUTRO);
	fprintf(stdout,"%svidas: %s%i%s pontos: %s%3i", MAGENTA,NEUTRO, g_jogo->vidas, MAGENTA, NEUTRO, g_jogo->pontos);
	fprintf(stdout,"%s%s%s\n", AZUL,"|", NEUTRO);
	fprintf(stdout,"%s%s%s\n", AZUL, "|____________________|", NEUTRO);
}

void uhet_show_game(void* fonte, char* str){

	int i = 0;
	while (str[i]) glutBitmapCharacter(fonte, str[i++]);
}

void uhet_show_game_fim(){


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	    glColor3f(0.0, 1.0, 0.0);
		glPushMatrix();
			glRasterPos2f(0.0, 0.0);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "GAME OVER\0");
		glPopMatrix();
	glPopMatrix();

	glutPostRedisplay(); //atualiza a tela

	sleep(1);
	glutLeaveMainLoop();
}

void uhet_show_game_help(){

	glPushMatrix();
	    glColor3f(0.0, 1.0, 0.0);
		glPushMatrix();
			glRasterPos2f(-0.9, 0.0);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "Ajuda:\0");
			glRasterPos2f(-0.9, -0.05);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "* h - ajuda e pause\0");
			glRasterPos2f(-0.9, -0.10);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "* lelt,down - move para esquerda\0");
			glRasterPos2f(-0.9, -0.15);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "* right,up - move para direita\0");
			glRasterPos2f(-0.9, -0.20);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "* esc - termina o jogo\0");
			glRasterPos2f(-0.9, -0.25);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "* a,d,w,s - controla a camera\0");
			glRasterPos2f(-0.9, -0.30);
			uhet_show_game(GLUT_BITMAP_8_BY_13, "* -,+ - controla o zoom\0");
		glPopMatrix();
	glPopMatrix();

}

void uhet_show_game_placar(){

	char str1[20];
	sprintf(str1,"Pontos: %i",g_jogo->pontos);
	char str2[20];
	sprintf(str2,"Vidas: %i",g_jogo->vidas);

	glPushMatrix();
	    glColor3f(0.0, 1.0, 0.0);
	    glRasterPos2f(-0.95, -0.95);
	    uhet_show_game(GLUT_BITMAP_8_BY_13, str1);
	    glRasterPos2f(0.8, -0.95);
	    uhet_show_game(GLUT_BITMAP_8_BY_13, str2);
	glPopMatrix();
}

void uhet_movimento_colisao_morte(){

	g_bola->posicao->x = 0;
	g_bola->posicao->y = 0;
	x = 1;
	y = 1;
	g_jogo->bola_sentido = CAINDO;
	g_jogo->bola_direcao = FALSE;
	g_jogo->bola_aceleracao = 0;
	g_jogo->vidas--;
//	if (g_jogo->vidas <= 0) glutLeaveMainLoop();
	if (g_jogo->vidas <= 0) g_jogo->n_blocos = 0;
	uhet_show_placar();
}

static void uhet_movimento_colisao(){

	float pos_x_bola = uhet_obj_box_get(g_bola,X_CENTRO);
	float pos_y_bola = uhet_obj_box_get(g_bola,Y_CENTRO);
	//chegou na altura do jogador
	if (pos_y_bola < uhet_obj_box_get(g_jogador,Y_INI)){
		
		//se a bola bateu no jagador
		float pos_xi_jogador = uhet_obj_box_get(g_jogador,X_INI);
		float pos_xf_jogador = uhet_obj_box_get(g_jogador,X_FIM);
		if ((pos_x_bola >= pos_xi_jogador) && ((pos_x_bola <= pos_xf_jogador))){

				g_jogo->bola_sentido = SUBINDO;
				system("aplay -q ./sound/chaos.wav 2> /dev/null &");
				float pos_colisao = pos_x_bola - pos_xi_jogador; 
				int porcentagem = (int) floor(pos_colisao*100 / (pos_xf_jogador - pos_xi_jogador));
				uhet_movimento_colisao_refletir(porcentagem);
				
		} else { //a bola nao bateu no jogador
			uhet_movimento_colisao_morte();
			system("aplay -q ./sound/damn.wav 2> /dev/null &");
		}
	} else if (pos_y_bola >= uhet_obj_box_get(g_borda,Y_INI)){//chegou no teto

			g_jogo->bola_sentido = CAINDO;
			system("aplay -q ./sound/chaos.wav 2> /dev/null&");

	} else if (pos_x_bola <= uhet_obj_box_get(g_borda,X_INI)){//chegou no lado esquerdo
		
//			uhet_show_msg("bateu na esquerda\n");
			g_jogo->bola_direcao = DIREITA;

			system("aplay -q ./sound/chaos.wav 2> /dev/null&");

	} else if (pos_x_bola >= uhet_obj_box_get(g_borda,X_FIM)){//chegou no lado direito

//			uhet_show_msg("bateu na direita\n");
			g_jogo->bola_direcao = ESQUERDA;
	
			system("aplay -q ./sound/chaos.wav 2> /dev/null&");
	} else //se pah, chego nos broco e talz!
		uhet_movimento_colisao_broco();
}


static void uhet_ctrl_idle(){

	if (!g_jogo->pause){
		usleep(10000);
		uhet_movimento_bola();
		uhet_movimento_colisao();
		glutPostRedisplay();
	}
}

void uhet_ctrl_fim(){

	uhet_show_placar();
	uhet_show_msg("maravilha alberto!\n");
	uhet_show_game_fim();
}


int uhet_ctrl_gogo(){

//	uhet_show_msg("uhet_gogo\n");
	g_jogo = uhet_obj_jogo_constructor();
	g_jogo->x = rint(glutGet(GLUT_SCREEN_WIDTH)/2) - (LARGURA/2);
	g_jogo->y = rint(glutGet(GLUT_SCREEN_HEIGHT)/2) - (ALTURA/2);

	g_camera = uhet_obj_camera_constructor();

	g_jogador = uhet_obj_box_contructor();
	uhet_obj_coordenadas_set(g_jogador->posicao,0,-34.0,0);
	uhet_obj_coordenadas_set(g_jogador->escala,0.3,0.025,1);
	uhet_obj_cor_set(g_jogador->cor_miolo,0.2,0.3,0.2,0.0);
	uhet_obj_cor_set(g_jogador->cor_borda,0.0,1.0,0.0,0.0);
	g_jogador->tamanho = 1.0;
	g_jogador->preenchido = TRUE;

	g_borda = uhet_obj_box_contructor();
	uhet_obj_coordenadas_set(g_borda->posicao,0,0.035,0.0);
	uhet_obj_coordenadas_set(g_borda->escala,1.97,1.84,0.0);
	uhet_obj_cor_set(g_borda->cor_borda,0.0,1.0,0.0,0.0);
	g_borda->tamanho = 1.0;
	g_borda->preenchido = FALSE;

	g_placar = uhet_obj_box_contructor();
	uhet_obj_coordenadas_set(g_placar->posicao,0,-9.3,0.0);
	uhet_obj_coordenadas_set(g_placar->escala,1.97,0.1,1.0);
	uhet_obj_cor_set(g_placar->cor_miolo,0.2,0.3,0.2,0.0);
	uhet_obj_cor_set(g_placar->cor_borda,0.0,1.0,0.0,0.0);
	g_placar->tamanho = 1.0;
	g_placar->preenchido = TRUE;

	g_bola = uhet_obj_box_contructor();
	uhet_obj_coordenadas_set(g_bola->posicao,0.0,0.0,0.0);
	uhet_obj_coordenadas_set(g_bola->escala,0.01,0.01,0.01);
	uhet_obj_cor_set(g_bola->cor_miolo,1.0,0.0,0,0.0);
	uhet_obj_cor_set(g_bola->cor_borda,1.0,0.0,0,0.0);
	g_bola->tamanho = 1.0;
	g_bola->preenchido = TRUE;

	uhet_obj_array_contructor();
	int i;
	for (i = 0 ; i < 10; i++){

		ga_blocos[i] = uhet_obj_box_contructor();
		uhet_obj_coordenadas_set(ga_blocos[i]->posicao,i-5,10,0.0);
		uhet_obj_coordenadas_set(ga_blocos[i]->escala,0.125,0.08,1.0);
		uhet_obj_cor_set(ga_blocos[i]->cor_borda,0.0,1.0,0.0,0.0);
		ga_blocos[i]->tamanho = 1.0;
		ga_blocos[i]->preenchido = FALSE;
		g_jogo->n_blocos++;

	}

	x = 1;
	y = 1;
	ORTO = 1;
	return 0;
}



int main(int argc, char* argv[]){

//	uhet_show_msg("lets vamos!\n");

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);

	uhet_ctrl_gogo();

	glutInitWindowSize (g_jogo->largura, g_jogo->altura);
//	printf("uhet: %i x %i\n", glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y));
	glutInitWindowPosition (g_jogo->x, g_jogo->y);
	glutCreateWindow(g_jogo->nome->str);

	glutDisplayFunc(uhet_ctrl_tela);
	glutKeyboardFunc(uhet_ctrl_teclado);
	glutSpecialFunc(uhet_ctrl_especial);
 	glutReshapeFunc(uhet_ctrl_reshape);
	glutIdleFunc(uhet_ctrl_idle);
	glutCloseFunc(uhet_ctrl_fim);

	glutMainLoopEvent();
	glutMainLoop();

//	uhet_show_msg("maravilha alberto!\n");
	return 0;
}
