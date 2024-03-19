#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#define UTILS_IMPL
#include "utils.h"

#define UI_IMPL
#include "ui.h"

#define Vec2 Vector2

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
      Square *src_s = &game->buf[src_x][src_y];
      Moves *moves = &game->moves[src_x][src_y];

      moves->num_moves = 0;

      if (src_s->type != P_SOLDIER && src_s->type != P_KING) continue;
      if (src_s->color != game->turn_num % 2) continue;
      if (game->turn_num == 1 && src_s->type == P_KING) continue;

      int offs[8][2] = {
        { -1, 0 },
        { -1, -1 },
        { 0, -1 },
        { 1, -1 },
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { -1, 1 },
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

        if (game->buf[x][y].win && src_s->type != P_KING) continue;

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

void draw_square(Game *game, int x, int y, float unit, Color s_color) {
  // Square
  if (s_color.a == 0) {
    s_color = square_colors[(x + y + 1) % 2];
  }
  if (game->buf[x][y].win) {
    s_color = GOLD;
  }

  DrawRectangle(
    x * unit, y * unit,
    unit, unit,
    s_color
  );

  // Piece
  float pos_x = (x + 0.5) * unit;
  float pos_y = (y + 0.5) * unit;
  float size = unit * 0.8;
  Color bcolor = piece_colors[0];
  Color pcolor = piece_colors[game->buf[x][y].color];

  Rectangle rect = {
    .x = pos_x - size/2,
    .y = pos_y - size/2,
    .width = size,
    .height = size,
  };

  if (game->buf[x][y].type == P_SOLDIER) {
    DrawCircle(pos_x, pos_y, size/2, pcolor);
    rlSetLineWidth(2);
    DrawCircleLines(pos_x, pos_y, size/2, bcolor);
  }

  if (game->buf[x][y].type == P_KING) {
    DrawRectangleRec(rect, pcolor);
    DrawRectangleLinesEx(rect, 2, bcolor);
  }

  if (game->buf[x][y].type == P_PILLAR) {
    DrawRectangleRec(rect, BEIGE);
    DrawRectangleLinesEx(rect, 2, bcolor);
  }
}        

int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);

  InitWindow(800, 600, "King's Valley");
  SetTargetFPS(60);
  SetExitKey(KEY_Q);

  Game game;
  char *fen = "sssKsss//1p3p1/3v3/1p3p1//SSSkSSS";
  fill_from_fen(&game, fen);

  enum {
    MODE_SRC,
    MODE_TARGET,
    MODE_END_MENU,
  } mode = MODE_END_MENU;

  Vec2I src = { 0, 0, };
  Vec2I sel = { 0, 0, };

  int win_width = 0, win_height = 0;
  float board_size = 0;

  bool initial = true; // This is a terrible solution

  while (!WindowShouldClose()) {
    if (IsWindowResized() || initial) {
      win_width = GetScreenWidth();
      win_height = GetScreenHeight();
      board_size = min(win_width, win_height);
      reg_font = LoadFontEx("recs/regular.ttf", board_size/30, 0, 0);
    }

    /* ===== Input ===== */
    {
      Vec2I vel = { 0, 0, };

      if (IsKeyPressed(KEY_H)) vel.x -= 1;
      if (IsKeyPressed(KEY_J)) vel.y += 1;
      if (IsKeyPressed(KEY_K)) vel.y -= 1;
      if (IsKeyPressed(KEY_L)) vel.x += 1;

      if (in_bounds(sel.x + vel.x, BOARD_WIDTH)) sel.x += vel.x;
      if (in_bounds(sel.y + vel.y, BOARD_HEIGHT)) sel.y += vel.y;

      switch (mode) {
      case MODE_SRC: {
        if (IsKeyPressed(KEY_SPACE)) {
          src.x = sel.x;
          src.y = sel.y;

          gen_moves(&game);
          Moves *moves = &game.moves[src.x][src.y];
          if (moves->num_moves > 0) {
            mode = MODE_TARGET;
          }
        }
      } break;

      case MODE_TARGET: {
        if (IsKeyPressed(KEY_SPACE)) {
          Moves *moves = &game.moves[src.x][src.y];

          bool found = false;

          for (int i = 0; i < moves->num_moves; i++) {
            if (moves->c[i][0] == sel.x && moves->c[i][1] == sel.y) {
              found = true;
            }
          }

          if (found) {
            Square *new = &game.buf[sel.x][sel.y];

            new->type = game.buf[src.x][src.y].type;
            new->color = game.buf[src.x][src.y].color;

            game.buf[src.x][src.y].type = 0;
            game.buf[src.x][src.y].color = -1;

            mode = MODE_SRC;
            game.turn_num += 1;

            // Check win
            if (new->type == P_KING && new->win) mode = MODE_END_MENU;
          }
          break;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
          mode = MODE_SRC;
        }
      } break;
      }
    }

    /* ===== Drawing ===== */
    BeginDrawing();
    ClearBackground(BG_COLOR);
    {
      float unit = min(
        win_width / BOARD_WIDTH,
        win_height / BOARD_HEIGHT
      );

      for (int x = 0; x < BOARD_WIDTH; x++) {
        for (int y = 0; y < BOARD_HEIGHT; y++) {
          draw_square(&game, x, y, unit, BLANK);
        }
      }

      if (mode == MODE_TARGET) {
        Color src_color = { 0, 255, 0, 100, };
        draw_square(&game, src.x, src.y, unit, src_color);

        Moves *moves = &game.moves[src.x][src.y];
        for (int i = 0; i < moves->num_moves; i++) {
          DrawCircle(
            (moves->c[i][0] + 0.5) * unit,
            (moves->c[i][1] + 0.5) * unit,
            unit * 0.2,
            (Color) { 50, 50, 50, 50, }
          );
        }
      }

      if (mode == MODE_TARGET || mode == MODE_SRC) {
        Rectangle rect = {
          .x = sel.x * unit,
          .y = sel.y * unit,
          .width = unit,
          .height = unit,
        };

        DrawRectangleLinesEx(
          rect,
          4,
          RED
        );
      }

      if (mode == MODE_END_MENU) {
        Rect rect = {
          .size.x = board_size * 0.7,
          .size.y = board_size * 0.5,
        };
        rect.pos.x = (win_width - rect.size.x)/2;
        rect.pos.y = (win_height - rect.size.y)/2;

        DrawRectangleRounded(
          rect_to_rectangle(&rect),
          0.1,
          20,
          (Color)
          { 50, 50, 50, 255, }
        );

        // DrawTextEx(
        //   reg_font,
        //   "You won! ... or maybe not",
        //   // todo: center text properly
        //   (Vec2){ rect.x + rect.width/10, rect.y + rect.height/10 },
        //   reg_font.baseSize,
        //   1,
        //   WHITE
        // );
        draw_text_rel(
          "You won! ... or maybe not",
          0.5, 0.15,
          reg_font,
          WHITE,
          rect
        );

        ButtonList list = {
          .count = 0,
          .cap = 100,
          .size = { rect.size.x/3, rect.size.y/5, },
          .spacing = rect.size.y/50,
        };
        da_init(&list);

        Button new_game = {
          .text = "New game",
          .pos = place_rel(0.5, 0.4, list.size, rect),
        };
        push_button(&list, &new_game);

        Button leave = { .text = "Exit", };
        push_button(&list, &leave);

        for (int i = 0; i < list.count; i++) {
          draw_button(list.items[i]);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
          if (mouse_in(&new_game)) {
            fill_from_fen(&game, fen);
            mode = MODE_SRC;
            sel.x = sel.y = 0;
          }

          if (mouse_in(&leave)) exit(0);
        }
      }
    }
    EndDrawing();

    initial = false;
  }

  CloseWindow();
  return 0;
}
