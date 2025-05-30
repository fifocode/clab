#include "console.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../crun/runtime.h"

void clear_console(void) {
  printf("\033[H\033[J");  // ANSI escape code to clear the screen
}

void set_cursor_position(int x, int y) {
  printf("\033[%d;%dH", y, x);  // ANSI escape code to set cursor position
}

void render(void *context) {
  Frame *frame = (Frame *)context;
  int x = 1;  // Adjust the horizontal position as needed
  int y = 1;  // Adjust the vertical position as needed

  clear_console();
  set_cursor_position(x, y);  // ANSI escape code to set cursor position
  printf("%s\n", frame->text);
  fflush(stdout);               // Ensure the output is printed immediately
  usleep(frame->ftime * 1000);  // Delay for specified microseconds
  clear_console();
  free(frame->text);
  free(frame);
}

Console *console_create(size_t ftime_mills) {
  Runtime *runtime = runtime_create();
  Console *console = (Console *)malloc(sizeof(Console));
  console->runtime = runtime;
  console->ftime = ftime_mills;
  return console;
}

void console_render(Console *console, char *text) {
  Frame *frame = (Frame *)malloc(sizeof(Frame));
  frame->ftime = console->ftime;
  frame->text = text;
  runtime_exec(console->runtime, render, frame);
}

void console_destroy(Console *console) {
  runtime_join_destroy(console->runtime);
  free(console);
}
