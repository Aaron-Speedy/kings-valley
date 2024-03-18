#ifndef UI_H
#define UI_H

typedef struct {
  Vector2 pos;
  Vector2 size;
  char *text;
} Button;

typedef struct {
  Button **items;
  size_t count, cap;
  Vector2 size;
  float spacing;
} ButtonList;

void draw_button(Button *button);
void push_button(ButtonList *list, Button *button);
bool mouse_in(Button *button);

#ifdef UI_H

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

Font regular_font;

void draw_button(Button *button) {
  Rectangle rect = {
    .width = button->size.x,
    .height = button->size.y,
    .x = button->pos.x,
    .y = button->pos.y,
  };

  DrawRectangleRounded(rect, 0.1, 20, WHITE);

  DrawTextEx(
    regular_font,
    button->text,
    // todo: center text properly
    (Vector2){ rect.x + rect.width/10, rect.y + rect.height/3 },
    regular_font.baseSize,
    1,
    BLACK
  );
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
  Vector2 v = GetMousePosition();
  if (v.x < button->pos.x) return 0;
  if (v.y < button->pos.y) return 0;
  if (v.x > button->pos.x + button->size.x) return 0;
  if (v.y > button->pos.y + button->size.y) return 0;
  return 1;
}

#endif

#endif
