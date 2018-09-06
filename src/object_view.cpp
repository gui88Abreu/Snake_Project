/*
Nome: Guilherme de Brito Abreu
RA: 173691
*/

#include "../include/model/model.hpp"
#include "../include/view/view.hpp"
#include "../include/controler/controler.hpp"

Tela::Tela(ListaDeSnakes *lds, int maxI, int maxJ, float maxX, float maxY) {
  this->lista = lds;
  this->lista_anterior = new ListaDeSnakes();
  this->lista_anterior->hard_copy(this->lista);
  this->maxI = maxI;
  this->maxJ = maxJ;
  this->maxX = maxX;
  this->maxY = maxY;
}

void Tela::init() {
  initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
  curs_set(0);           /* Do not display cursor */
}

void Tela::update() {
  std::vector<Snake*> *s_old = this->lista_anterior->get_snakes();
  std::vector<Snake*> *s = this->lista->get_snakes();

  for (int k =0; k < s_old->size(); k++){
    int i, j;
    std::vector<Corpo *> *corpos_old = (*s_old)[k]->get_corpos();
    std::vector<Corpo *> *corpos = (*s)[k]->get_corpos();

    // Apaga corpos na tela
    for (int z = 0; z < corpos_old->size(); z++)
    {
      pos_2d p = (*corpos_old)[z]->get_posicao();
      i = (int)p.y * (this->maxI / this->maxX);
      j = (int)p.x * (this->maxI / this->maxX);
      move(i,j);   /* Move cursor to position */
      echochar(' ');  /* Prints character, advances a position */
    }

    // Desenha corpos na tela
    for (int z = 0; z < corpos->size(); z++)
    {
      pos_2d p = (*corpos)[z]->get_posicao();
      i = (int)p.y * (this->maxI / this->maxX);
      j = (int)p.x * (this->maxI / this->maxX);
      move(i,j);   /* Move cursor to position */
      echochar('*');  /* Prints character, advances a position */

      // Atualiza corpos antigos
      (*corpos_old)[z]->update( (*corpos)[z]->get_velocidade(),(*corpos)[z]->get_posicao());
    }
  }

  // Atualiza tela
  refresh();
}

void Tela::stop() {
  endwin();
}

Tela::~Tela() {
  this->stop();;
}

void threadfun (char *keybuffer, int *control)
{
  char c;
  while ((*control) == 1) {
    c = getch();
    if (c!=ERR) (*keybuffer) = c;
    else (*keybuffer) = 0;
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
  }
  return;
}

Teclado::Teclado(){
}

Teclado::~Teclado() {
}

void Teclado::init() {
  // Inicializa ncurses
  raw();				         /* Line buffering disabled	*/
	keypad(stdscr, TRUE);	 /* We get F1, F2 etc..		*/
	noecho();			         /* Don't echo() while we do getch */
  curs_set(0);           /* Do not display cursor */

  this->rodando = 1;
  std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
  (this->kb_thread).swap(newthread);
}

void Teclado::stop() {
  this->rodando = 0;
  (this->kb_thread).join();
}

char Teclado::getchar() {
  char c = this->ultima_captura;
  this->ultima_captura = 0;
  return c;
}