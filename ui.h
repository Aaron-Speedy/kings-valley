#ifndef UI_H
#define UI_H

#define Vec2 Vector2

typedef struct {
  Vec2 pos;
  Vec2 size;
  char *text;
} Button;

typedef struct {
  Button **items;
  size_t count, cap;
  Vec2 size;
  float spacing;
} ButtonList;

typedef struct {
  Vec2 pos;
  Vec2 size;
} Rect;

Vec2 place_rel(float x, float y, Vec2 size, Rect space);
Rectangle rect_to_rectangle(Rect *rect);
void draw_text_rel(char *text, float x, float y, Font font, Color color, Rect space);
void draw_button(Button *button);
void push_button(ButtonList *list, Button *button);
bool mouse_in(Button *button);

#ifdef UI_IMPL

#define da_append(xs, x) \
  do { \
    if ((xs)->count >= (xs)->cap) { \
      if ((xs)->cap == 0) (xs)->cap = 256; \
      else (xs)->cap *= 2; \
      (xs)->items = realloc((xs)->items, (xs)->cap*sizeof(*(xs)->items)); \
    } \
 \
    (xs)->items[(xs)->count++] = (x); \
  } while (0)

#define da_last(xs) (assert(xs->count > 0), (xs)->items[(xs)->count - 1])

Font reg_font;

Vec2 place_rel(float x, float y, Vec2 size, Rect space) {
  return (Vec2) {
    .x = space.pos.x + x * space.size.x - size.x / 2,
    .y = space.pos.y + y * space.size.y - size.y / 2,
  };
}

Rectangle rect_to_rectangle(Rect *rect) {
  return (Rectangle) {
    .x = rect->pos.x,
    .y = rect->pos.y,
    .width = rect->size.x,
    .height = rect->size.y,
  };
}

void draw_text_rel(char *text, float x, float y, Font font, Color color, Rect space) {
  DrawTextEx(
    font,
    text,
    place_rel(
      x, y,
      MeasureTextEx(font, text, font.baseSize, 1),
      space
    ),
    reg_font.baseSize,
    1,
    color
  );
}

void draw_button(Button *button) {
  Rect rect = {
    .pos = button->pos,
    .size = button->size,
  };

  Color button_color = WHITE;

  if (mouse_in(button)) {
    float factor = 0.75;
    button_color.r *= factor;
    button_color.g *= factor;
    button_color.b *= factor;
  }

  DrawRectangleRounded(rect_to_rectangle(&rect), 0.1, 20, button_color);

  draw_text_rel(button->text, 0.5, 0.5, reg_font, BLACK, rect);
}

void push_button(ButtonList *list, Button *button) {
  button->size = list->size;
  if (list->count > 0) {
    button->pos = da_last(list)->pos;
    button->pos.y += list->size.y + list->spacing;
  }
  da_append(list, button);
}

bool mouse_in(Button *button) {
  Vec2 v = GetMousePosition();
  if (v.x < button->pos.x) return 0;
  if (v.y < button->pos.y) return 0;
  if (v.x > button->pos.x + button->size.x) return 0;
  if (v.y > button->pos.y + button->size.y) return 0;
  return 1;
}

#endif

#endif
