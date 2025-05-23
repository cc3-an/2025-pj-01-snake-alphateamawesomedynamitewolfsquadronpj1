#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
game_state_t* create_default_state() {
  game_state_t *initial_state = malloc(sizeof(game_state_t));
  if (!initial_state){
    return NULL;
  }
  initial_state->num_rows = 18;
  initial_state->board = malloc(sizeof(char *) * initial_state->num_rows);
  if (!initial_state->board) {
      free(initial_state);
      return NULL;
  }

  const char *predefined_board[18] = {
      "####################",
      "#                  #",
      "# d>D    *         #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "#                  #",
      "####################"
  };

  for (int i = 0; i < initial_state->num_rows; i++) {
      size_t len = strlen(predefined_board[i]) + 1;
      initial_state->board[i] = malloc(len);
      if (!initial_state->board[i]) {
          for (int j = 0; j < i; j++) free(initial_state->board[j]);
          free(initial_state->board);
          free(initial_state);
          return NULL;
      }
      strcpy(initial_state->board[i], predefined_board[i]);
  }

  initial_state->num_snakes = 1;
  initial_state->snakes = malloc(sizeof(snake_t));
  if (!initial_state->snakes) {
      for (int i = 0; i < initial_state->num_rows; i++) free(initial_state->board[i]);
      free(initial_state->board);
      free(initial_state);
      return NULL;
  }

  snake_t *snake = &initial_state->snakes[0];
  snake->live = true;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->tail_row = 2;
  snake->tail_col = 2;

  return initial_state;
}

/* Tarea 2 */
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
void free_state(game_state_t* state) {
  for (int i = 0; i < state->num_rows; i++)
  {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Tarea 3 */
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
void print_board(game_state_t *state, FILE *fp) {
  if (!state || !fp) {
    return;
  }
  int i = 0;
  unsigned int total_rows = state->num_rows;
  while (i < total_rows) {
      fprintf(fp, "%s\n", state->board[i]);
      i++;
  }
}

/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */
/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static bool is_tail(char c) {
  if (c == 'w' || c == 'a' || c == 'd' || c == 's')
  {
    return true;
  }

  return false;
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static bool is_head(char c) {
  if (c == 'W' || c == 'A' || c == 'D' || c == 'S' || c == 'x')
  {
    return true;
  }

  return false;
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static bool is_snake(char c) {
  if (is_head(c) || is_tail(c) || c == '^' || c == '<' || c == 'v' || c == '>')
  {
    return true;
  }
  return false;
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static char body_to_tail(char c) {
  switch (c) {
    case '^': return 'w';
    case '<': return 'a';
    case 'v': return 's';
    case '>': return 'd';
    default:  return '?';
  }
}



/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static char head_to_body(char c) {
  switch (c) {
    case 'W': return '^';
    case 'A': return '<';
    case 'S': return 'v';
    case 'D': return '>';
    default:  return '?';
  }
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S')
  {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W')
  {
    return cur_row - 1;
  }else{
    return cur_row;
  }
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if(c == '>' ||c == 'd' ||c == 'D')
  {
    return cur_col +1;
  } else if(c == '<' ||c == 'a' ||c == 'A')
  {
    return cur_col -1;
  } else {
    return cur_col;
  }
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static char next_square(game_state_t* state, unsigned int snum) {
  snake_t serpiente = state->snakes[snum];
  char cabeza = get_board_at(state,serpiente.head_row, serpiente.head_col);
  unsigned int cabeza_row = get_next_row(serpiente.head_row, cabeza);
  unsigned int cabeza_col = get_next_col(serpiente.head_col, cabeza);
  return get_board_at(state, cabeza_row, cabeza_col);
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static void update_head(game_state_t* state, unsigned int snum) {
  snake_t* serpiente = &state->snakes[snum];
  char cabeza = get_board_at(state,serpiente->head_row, serpiente->head_col);
  unsigned int cabeza_row = serpiente->head_row;
  unsigned int cabeza_col = serpiente->head_col;
  if (cabeza == 'D')
  {
    set_board_at(state,cabeza_row,cabeza_col + 1, 'D');
    set_board_at(state,cabeza_row,cabeza_col, '>');
    serpiente->head_col +=1;
  } else if (cabeza == 'A')
  {
    set_board_at(state,cabeza_row,cabeza_col-1, 'A');
    set_board_at(state,cabeza_row,cabeza_col, '<');
    serpiente->head_col -=1;
  } else if (cabeza == 'W')
  {
    set_board_at(state,cabeza_row-1,cabeza_col, 'W');
    set_board_at(state,cabeza_row,cabeza_col, '^');
    serpiente->head_row -=1;
  } else if (cabeza == 'S')
  {
    set_board_at(state,cabeza_row+1,cabeza_col, 'S');
    set_board_at(state,cabeza_row,cabeza_col, 'v');
    serpiente->head_row +=1;
  }
  return;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static void update_tail(game_state_t* state, unsigned int snum) {
  snake_t* serpiente = &state->snakes[snum];
  char cola = get_board_at(state,serpiente->tail_row, serpiente->tail_col);
  unsigned int cola_row = serpiente->tail_row;
  unsigned int cola_col = serpiente->tail_col;
  if (cola == 'd'){
    set_board_at(state,cola_row,cola_col, ' ');
    cola_col +=1;
  } else if (cola == 'a'){
    set_board_at(state,cola_row,cola_col, ' ');
    cola_col -=1;
  } else if (cola == 'w'){
    set_board_at(state,cola_row,cola_col, ' ');
    cola_row -=1;
  } else if (cola == 's'){
    set_board_at(state,cola_row,cola_col, ' ');
    cola_row+=1;
  }
  char siguiente = get_board_at(state, cola_row, cola_col);
  if (siguiente == '>'){
    set_board_at(state, cola_row, cola_col, 'd');
  }else if (siguiente == '<'){
    set_board_at(state, cola_row, cola_col, 'a');
  }else if (siguiente == '^'){
    set_board_at(state, cola_row, cola_col, 'w');
  }else if (siguiente == 'v'){
    set_board_at(state, cola_row, cola_col, 's');
  }
  serpiente->tail_row = cola_row;
  serpiente->tail_col = cola_col;
  return;
}

/* Tarea 4.5 */
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  for (unsigned i = 0; i < state->num_snakes; i++)
  {
    snake_t* serpiente_actual = &state->snakes[i];
    if (serpiente_actual->live == true){
      char siguiente_espacio = next_square(state, i);
      if (siguiente_espacio == '*'){
        update_head(state, i);
        add_food(state);
      } else if (siguiente_espacio == ' '){
        update_head(state, i);
        update_tail(state,i);
      } else if (siguiente_espacio == '#' || is_snake(siguiente_espacio)){
        serpiente_actual->live = false;
        unsigned int cabeza_row = serpiente_actual->head_row;
        unsigned int cabeza_col = serpiente_actual->head_col;
        set_board_at(state, cabeza_row, cabeza_col, 'x');
      }
    }
  }
  return;
}

/* Tarea 5 */
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
game_state_t* load_board(char* filename) {
  FILE* archivo = fopen(filename, "r");
  if (archivo == NULL) return NULL;

  game_state_t* state = malloc(sizeof(game_state_t));
  if (state == NULL) return NULL;

  state->num_snakes = 0;
  state->snakes = NULL;

  unsigned int filas_capacidad = 8;
  state->board = malloc(filas_capacidad * sizeof(char*));
  if (state->board == NULL) {
      free(state);
      return NULL;
  }
  state->num_rows = 0;

  unsigned int col_capacidad = 8;
  char* fila_actual = malloc(col_capacidad);
  if (fila_actual == NULL) {
      free(state->board);
      free(state);
      return NULL;
  }
  unsigned int col = 0;

  int c;
  while ((c = fgetc(archivo)) != EOF) {
      if (c == '\n') {
          if (col >= col_capacidad) {
              col_capacidad++;
              fila_actual = realloc(fila_actual, col_capacidad);
              if (fila_actual == NULL) return NULL;
          }
          fila_actual[col] = '\0';

          if (state->num_rows >= filas_capacidad) {
              filas_capacidad *= 2;
              char** tmp = realloc(state->board, filas_capacidad * sizeof(char*));
              if (tmp == NULL) return NULL;
              state->board = tmp;
          }

          state->board[state->num_rows++] = fila_actual;

          col_capacidad = 8;
          fila_actual = malloc(col_capacidad);
          if (fila_actual == NULL) return NULL;
          col = 0;
      } else {
          if (col >= col_capacidad - 1) {
              col_capacidad *= 2;
              char* tmp = realloc(fila_actual, col_capacidad);
              if (tmp == NULL) return NULL;
              fila_actual = tmp;
          }
          fila_actual[col++] = (char)c;
      }
  }

  if (col > 0) {
      if (col >= col_capacidad) {
          col_capacidad++;
          fila_actual = realloc(fila_actual, col_capacidad);
      }
      fila_actual[col] = '\0';

      if (state->num_rows >= filas_capacidad) {
          filas_capacidad *= 2;
          state->board = realloc(state->board, filas_capacidad * sizeof(char*));
      }
      state->board[state->num_rows++] = fila_actual;
  } else {
      free(fila_actual);
  }

  fclose(archivo);
  return state;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
static void find_head(game_state_t* state, unsigned int snum) {
  snake_t* snake = &state->snakes[snum];
  unsigned int cola_row = snake->tail_row;
  unsigned int cola_col = snake->tail_col;
  char c = state->board[cola_row][cola_col];

  while (!is_head(c)) {
        cola_row = get_next_row(cola_row, c);
        cola_col = get_next_col(cola_col, c);
        c = state->board[cola_row][cola_col];
  }
  snake->head_row = cola_row;
  snake->head_col = cola_col;
}


/* Tarea 6.2 */
/************************************************************
 *  Desarolladores                                          *
 *    - María Claudia Lainfiesta Herrera - 24000149         *
 *    - Adriel Levi Argueta Caal - 24003171                 *
 *                                                          *
 *  Proyecto: C Snake                                       *
 *  Equipo: Alpha Team Awesome Dynamite Wolf Squadron       *
 ************************************************************/
game_state_t* initialize_snakes(game_state_t* state) {
  if (state == NULL) {
      return NULL;
  }
  unsigned int snake_count = 0;
  for (unsigned int i = 0; i < state->num_rows; i++) {
      for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
          if (is_tail(state->board[i][j])) {
              snake_count++;
          }
      }
  }
  state->snakes = malloc(sizeof(snake_t) * snake_count);
  if (state->snakes == NULL) {
      return NULL;
  }
  state->num_snakes = snake_count;
  unsigned int indice = 0;
  for (unsigned int i = 0; i < state->num_rows; i++) {
      for (unsigned int j = 0; state->board[i][j] != '\0'; j++) {
          if (is_tail(state->board[i][j])) {
              state->snakes[indice].tail_row = i;
              state->snakes[indice].tail_col = j;
              state->snakes[indice].live = true;
              find_head(state, indice);
              indice++;
          }
      }
  }
  return state;
}
