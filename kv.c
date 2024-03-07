#include <raylib.h>
#include <rlgl.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#define UTILS_IMPL
#include "utils.h"

#define BOARD_WIDTH 7
#define BOARD_HEIGHT 7

Color BG_COLOR = { 22, 21, 18, 255, };

Color square_colors[2] = {
  { 181, 136, 99, 255, },
  { 240, 217, 181, 255, },
};

Color piece_colors[2] = {
  { 0, 0, 0, 255, },
  { 255, 255, 255, 255, },
};

typedef struct {
  enum {
    P_EMPTY = 0,
    P_PILLAR,
    P_SOLDIER,
    P_KING,
  } type;
  bool color;
  bool win;
} Square;

#define MAX_MOVES 8

typedef struct {
  int c[MAX_MOVES][2];
  int num_moves;
} Moves;

typedef struct {
  Square buf[BOARD_WIDTH][BOARD_HEIGHT];
  Moves moves[BOARD_WIDTH][BOARD_HEIGHT];
  int turn_num;
} Game;

// TODO: Lose if surrounded; win if on valley
void gen_moves(Game *game) {
  for (int src_x = 0; src_x < BOARD_WIDTH; src_x++) {
    for (int src_y = 0; src_y < BOARD_HEIGHT; src_y++) {
      Square *src = &game->buf[src_x][src_y];
      Moves *moves = &game->moves[src_x][src_y];

      moves->num_moves = 0;

      if (src->type != P_SOLDIER && src->type != P_KING) continue;
      if (src->color != game->turn_num % 2) continue;
      if (game->turn_num == 1 && src->type == P_KING) continue;

      int offs[8][2] = {
        { 0, 1 },
        { 0, -1 },
        { 1, 0 },
        { -1, 0 },
        { 1, 1 },
        { 1, -1 },
        { -1, 1 },
        { -1, -1 },
      };
      for (int i = 0; i < 8; i++) {
        int x = src_x;
        int y = src_y;

        while (true) {
          x += offs[i][0];
          y += offs[i][1];

          if (!in_bounds(x, BOARD_WIDTH)) break;
          if (!in_bounds(y, BOARD_HEIGHT)) break;
          if (game->buf[x][y].type) break;
        }

        x -= offs[i][0];
        y -= offs[i][1];

        if (game->buf[x][y].win && src->type != P_KING) break;

        if (x != src_x || y != src_y) {
          moves->c[moves->num_moves][0] = x;
          moves->c[moves->num_moves][1] = y;
          moves->num_moves += 1;
        }
      }

    }
  }
}

void fill_from_fen(Game *game, char *fen) {
  memset(game, 0, sizeof(*game));
  game->turn_num = 1;

  size_t fen_len = strlen(fen);
  int pos_x = 0, pos_y = 0;

  int num_place_val = 0;
  int num = 0;

  for (int i = 0; i < fen_len; i++) {
    if (fen[i] >= '0' && fen[i] <= '9') {
      num += num_place_val * (fen[i] - '0');
      num_place_val *= 10;
      continue;
    } else {
      if (num > 0) pos_x += num;
      num_place_val = 1;
      num = 0;
    }

    if (fen[i] == '/') {
      pos_x = 0;
      pos_y += 1;
      continue;
    }

    Square *cur_square = &game->buf[pos_x][pos_y];

    switch (fen[i]) {
    case 's': {
      cur_square->color = 0;
      cur_square->type = P_SOLDIER;
    } break;
    case 'k': {
      cur_square->color = 0;
      cur_square->type = P_KING;
    } break;
    case 'S': {
      cur_square->color = 1;
      cur_square->type = P_SOLDIER;
    } break;
    case 'K': {
      cur_square->color = 1;
      cur_square->type = P_KING;
    } break;
    case 'p': {
      cur_square->color = 0;
      cur_square->type = P_PILLAR;
    } break;
    case 'v': {
      cur_square->color = 0;
      cur_square->win = true;
    } break;
    }
    pos_x += 1;
  }
}

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  int window_width = 800, window_height = 600;
  InitWindow(window_width, window_height, "King's Valley");
  SetTargetFPS(60);
  SetExitKey(KEY_Q);

  Game game;
  fill_from_fen(&game, "sssksss//1p3p1/3v3/1p3p1//SSSKSSS");

  int move_index = 0;

  enum {
    MODE_SRC,
    MODE_TARGET,
  } mode = MODE_SRC;
  int src_x = 0, src_y = 0;
  int sel_x = 0, sel_y = 0;

  while (!WindowShouldClose()) {
    /* ===== Input ===== */
    {
      int vel_x = 0, vel_y = 0;

      if (IsKeyPressed(KEY_H)) vel_x -= 1;
      if (IsKeyPressed(KEY_J)) vel_y += 1;
      if (IsKeyPressed(KEY_K)) vel_y -= 1;
      if (IsKeyPressed(KEY_L)) vel_x += 1;

      switch (mode) {
      case MODE_SRC: {
        if (in_bounds(sel_x + vel_x, BOARD_WIDTH)) sel_x += vel_x;
        if (in_bounds(sel_y + vel_y, BOARD_HEIGHT)) sel_y += vel_y;

        if (IsKeyPressed(KEY_SPACE)) {
          gen_moves(&game);

          src_x = sel_x;
          src_y = sel_y;

          Moves *moves = &game.moves[src_x][src_y];

          if (moves->num_moves > 0) {
            mode = MODE_TARGET;
            move_index = 0;
            sel_x = moves->c[move_index][0];
            sel_y = moves->c[move_index][1];
          }
        }
      } break;

      case MODE_TARGET: {
        if (IsKeyPressed(KEY_SPACE)) {
          game.buf[sel_x][sel_y].type =
            game.buf[src_x][src_y].type;
          game.buf[sel_x][sel_y].color =
            game.buf[src_x][src_y].color;

          game.buf[src_x][src_y].type = 0;
          game.buf[src_x][src_y].color = -1;

          mode = MODE_SRC;
          game.turn_num += 1;
        }

        Moves *moves = &game.moves[src_x][src_y];
        if (in_bounds(move_index + vel_x, moves->num_moves)) {
          move_index += vel_x;
          sel_x = moves->c[move_index][0];
          sel_y = moves->c[move_index][1];
        }
      } break;
      }
    }
    /* ===== Drawing ===== */
    BeginDrawing();
    ClearBackground(BG_COLOR);
    {
      float unit = min(
        window_width / BOARD_WIDTH,
        window_height / BOARD_HEIGHT
      );

      for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
          DrawRectangle(
            x * unit, y * unit,
            unit, unit,
            square_colors[(x + y + 1) % 2]
          );

          if (game.buf[x][y].win) {
            DrawRectangle(
              x * unit, y * unit,
              unit, unit,
              GOLD
            );
          }
        }
      }

      {
        if (mode == MODE_TARGET) {
          DrawRectangle(
            src_x * unit,
            src_y * unit,
            unit,
            unit,
            (Color) { 0, 255, 0, 100, }
          );
        }

        Rectangle rect = {
          .x = sel_x * unit,
          .y = sel_y * unit,
          .width = unit,
          .height = unit,
        };

        DrawRectangleLinesEx(
          rect,
          4,
          RED
        );
      }

      for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
          float pos_x = (x + 0.5) * unit;
          float pos_y = (y + 0.5) * unit;
          float size = unit * 0.8;
          Color bcolor = piece_colors[0];
          Color pcolor = piece_colors[game.buf[x][y].color];

          Rectangle rect = {
            .x = pos_x - size/2,
            .y = pos_y - size/2,
            .width = size,
            .height = size,
          };

          if (game.buf[x][y].type == P_SOLDIER) {
            DrawCircle(pos_x, pos_y, size/2, pcolor);
            rlSetLineWidth(2);
            DrawCircleLines(pos_x, pos_y, size/2, bcolor);
          }

          if (game.buf[x][y].type == P_KING) {
            DrawRectangleRec(rect, pcolor);
            DrawRectangleLinesEx(rect, 2, bcolor);
          }

          if (game.buf[x][y].type == P_PILLAR) {
            DrawRectangleRec(rect, BEIGE);
            DrawRectangleLinesEx(rect, 2, bcolor);
          }
        }
      }

      if (mode == MODE_TARGET) {
        Moves *moves = &game.moves[src_x][src_y];
        for (int i = 0; i < moves->num_moves; i++) {
          DrawCircle(
            (moves->c[i][0] + 0.5) * unit,
            (moves->c[i][1] + 0.5) * unit,
            unit * 0.2,
            (Color) { 50, 50, 50, 50, }
          );
        }
      }
    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
