#include "pathdemo.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../crun/runtime.h"
#include "../dslib/datastr.h"
#include "../dslib/graph.h"
#include "../dslib/hashmap.h"
#include "../dslib/path.h"
#include "../dslib/util.h"
#include "../term/console.h"
#include "model.h"

/*-----------------------GRAPH 2D ARR DEMO-------------- */

int graph_2d_arr_demo(void) {
  printf("\n---------------GRAPH 2D ARR DEMO----------------\n");
  const char maze[] = "##########..@.#.@##@....G.##.#..@.@##.##@#####..@.S..##########";

  size_t rows = 7;
  size_t cols = 9;
  char ***arr = util_create_2d_str_arr(rows, cols, 5);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      char temp[50];
      char data = maze[i * cols + j];
      snprintf(temp, 50, "%c", data);
      strcpy(arr[i][j], temp);
      printf("%c", data);
    }
    printf("\n");
  }

  Graph2DMap *gmap = util_graph_from_2d_arr(arr, rows, cols);
  gmap->graph->debug = true;
  graph_print(gmap->graph, location_str_data_to_string);

  char *data = (char *)graph_get(gmap->graph, 40);
  printf("Graph found id %d : %c\n", 40, *data);

  data = (char *)graph_get(gmap->graph, 24);
  printf("Graph found id %d : %c\n", 24, *data);

  graph_destroy(gmap->graph, free_data_func);
  hashmap_destroy(gmap->idmap, free_data_func);
  util_destroy_2d_str_arr(arr, rows, cols);

  printf("\n---------------GRAPH 2D ARR DEMO----------------\n");
  return EXIT_SUCCESS;
}

/*-----------------------GRAPH 2D ARR DEMO-------------- */

/*-------SHORTEST PATH NON WEIGHTED GRAPH DEMO--------- */

/*
 [0]A-----[1]B-----[2]C-----[3]D-----[4]E
    |        |        |        |        |
    |        |        |        |        |
 [5]F-----[6]G-----[7]H-----[8]I-----[9]J
    |        |        |        |        |
    |        |        |        |        |
 10]K----[11]L----[12]M----[13]N----[14]O
    |        |        |        |        |
    |        |        |        |        |
[15]P----[16]Q----[17]R----[18]S----[19]T
    |        |        |        |        |
    |        |        |        |        |
[20]U----[21]V----[22]W----[23]X----[24]Y
 */
void path_shortest_console(void *arg) {
  Runtime *rnc = (Runtime *)arg;
  Console *console = console_create(100);
  clock_t start = clock();
  double cputime = 0;

  while (rnc->exit != true) {
    clock_t end = clock();
    cputime = ((double)(end - start)) / CLOCKS_PER_SEC;

    char *text = malloc(50 * sizeof(char));
    snprintf(text, 50, "processing %f sec", cputime);
    console_render(console, text);
    usleep(1000000);
  }

  console_destroy(console);

  clock_t end = clock();
  cputime = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Execution time %f\n sec", cputime);
}

void path_shortest_nwg_tree_solution(void *arg) {
  Runtime *rnc = (Runtime *)arg;

  // size_t rows = 30;
  // size_t cols = 30;

  size_t rows = 5;
  size_t cols = 5;

  char ***arr = util_create_2d_str_arr(rows, cols, 5);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      // char str[50];
      // size_t num = i * cols + j;
      // size_t(str, 50, "N%d", num);

      char str[2];
      char num = 65 + i * cols + j;
      snprintf(str, 2, "%c", num);

      strcpy(arr[i][j], str);
    }
  }

  // cannot auto free arr[i][j] since arr[i] is a continous memory
  Graph2DMap *gmap = util_graph_from_2d_arr(arr, rows, cols);
  gmap->graph->debug = false;

  // size_t srcid = hashmap_get(gmap->idmap, "N6");
  // size_t dstid = hashmap_get(gmap->idmap, "N300");

  size_t srcid = *(size_t *)hashmap_get(gmap->idmap, "I");
  size_t dstid = *(size_t *)hashmap_get(gmap->idmap, "U");

  Stack *stack = path_shortest_nwg_tree_vis(gmap->graph, srcid, dstid,
                                            graph_location_str_data_to_string);  // G->S

  runtime_join_destroy(rnc);
  graph_print(gmap->graph, location_str_data_to_string);
  stack_print(stack, location_str_data_to_string);

  stack_destroy(stack, NULL);
  graph_destroy(gmap->graph, free_data_func);
  hashmap_destroy(gmap->idmap, free_data_func);
  util_destroy_2d_str_arr(arr, rows, cols);
}

int path_shortest_nwg_tree_demo(void) {
  printf("\n-----SHORTEST PATH NON WEIGHTED GRAPH DEMO-----\n");

  Runtime *rns = runtime_create();
  Runtime *rnc = runtime_create();

  runtime_exec(rns, path_shortest_nwg_tree_solution, rnc);
  runtime_exec(rnc, path_shortest_console, rnc);

  runtime_join_destroy(rns);

  printf("\n-----SHORTEST PATH NON WEIGHTED GRAPH DEMO-----\n");
  return EXIT_SUCCESS;
}

/*-------SHORTEST PATH NON WEIGHTED GRAPH DEMO--------- */

/*------------SHORTEST PATH GRAPH DEMO--------------*/

/*
 [0]A-----[1]B-----[2]C-----[3]D-----[4]E
    |        |        |        |        |
    |        |        |        |        |
 [5]F-----[6]G-----[7]H-----[8]I-----[9]J
    |        |        |        |        |
    |        |        |        |        |
 10]K----[11]L----[12]M----[13]N----[14]O
    |        |        |        |        |
    |        |        |        |        |
[15]P----[16]Q----[17]R----[18]S----[19]T
    |        |        |        |        |
    |        |        |        |        |
[20]U----[21]V----[22]W----[23]X----[24]Y
 */
void path_shortest_solution(void *arg) {
  Runtime *rnc = (Runtime *)arg;

  size_t rows = 5;
  size_t cols = 5;

  char ***arr = util_create_2d_str_arr(rows, cols, 5);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      // char str[50];
      // size_t num = i * cols + j;
      // snprintf(str, 50, "N%zu", num);

      char str[2];
      char num = 65 + i * cols + j;
      snprintf(str, 2, "%c", num);
      strcpy(arr[i][j], str);
    }
  }

  Graph2DMap *gmap = util_graph_from_2d_arr(arr, rows, cols);
  gmap->graph->debug = false;

  size_t srcid = *(size_t *)hashmap_get(gmap->idmap, "I");
  size_t dstid = *(size_t *)hashmap_get(gmap->idmap, "U");

  Stack *stack = path_find_shortest(gmap->graph, srcid, dstid);

  runtime_join_destroy(rnc);
  graph_print(gmap->graph, location_str_data_to_string);
  stack_print(stack, location_str_data_to_string);

  stack_destroy(stack, free_data_func);
  graph_destroy(gmap->graph, free_data_func);
  hashmap_destroy(gmap->idmap, free_data_func);
  util_destroy_2d_str_arr(arr, rows, cols);
}

int path_shortest_demo(void) {
  printf("\n------------SHORTEST PATH GRAPH DEMO--------------\n");

  Runtime *rns = runtime_create();
  Runtime *rnc = runtime_create();

  runtime_exec(rns, path_shortest_solution, rnc);
  runtime_exec(rnc, path_shortest_console, rnc);

  runtime_join_destroy(rns);

  printf("\n------------SHORTEST PATH GRAPH DEMO--------------\n");
  return EXIT_SUCCESS;
}
/*------------SHORTEST PATH GRAPH DEMO--------------*/
