# clab: Creative Programming Solutions
![Banner](.extra/docs/clab-banner.png)

# About the project
clab project aims to provide creative solutions to interesting programming problems. 

C language has been chosen so that the project can develop solutions from scratch and in the process we will build the necessary data structures and algorithms. 

Also, a solution is very insightful when memory is at center stage. We get closer to the actual hardware and the operating system, getting the maximum performance.

Programming is enjoyable when applied to solve interesting problems (may not be directly used in general day-to-day professional work). The learning and knowledge acquired in the process make a better programmer.

## An Interesting Problem - Orienteering Challenge
Given a maze of m x n size
```
# boundary
. open path
@ checkpoint
S source
G destination
```

Develop a program to find the shortest path from 'S' to 'G', visiting all the '@'s.

Example: 7 x 9 maze
```
#########
#....#.@#
#.@...G.#
#.#.....#
#.##@####
#....S..#
#########
```

Solution:
```
Step 1:     Step 2:      Step 3:      Step 4:
#########   #########    #########    #########
#    #  #   #    #  #    #    #.1#    #    #.1#
#       #   # 2..   #    # 2...G #    #     G #
# #     #   # # .   #    # #     #    # #     #
# ##3####   # ##3####    # ## ####    # ## ####
#   .S  #   #       #    #       #    #       #
#########   #########    #########    #########

Maze: shortest travel distance 14
Time taken: 0.000932 sec
```

Background:
> I got this problem statement in my college placement, but I was not able to solve it in the given deadline. Later in the holidays I went home and was able to solve it ([solution link](https://github.com/janishar/orienteering-problem)), but with the limited knowledge it was difficult to optimize it. Now, after a decade its exciting to revisit the problem and take it as an opportunity to experiment with many other programming concepts.

### Solution pointers
This project develops as an experiement - We play with one part at a time and build it as a general solution through investigation.

- If there are N checkpoints the total possible path is N!. Factorial N gets very large with increase in N. For example: factorial 12 is 479,001,600. 
- To process such a large combination, we have to reduce memory space through better algorithms.
- We will use threads to do concurrent processing.
- Many data-structures will play a role in the solution: Graph, Stack, Queue, HashMap, LinkedList, Array, BufferQueue, Tree (for fun)
- The Maze conversion into a Graph and printing solution is also a good problem statement in itself.
- All the data-structures need to be thread safe, since we are using concurrent processing.

Note:
> We will not use recursion, since the function stack size is limited, and easily throws stackoverflow error. We will use Stack data-structure for computation in suce cases.

C does not have generics, but it has void pointer to reference any data type. We will use this to hold data in our data-structures. In this case we will have to pass function pointers to free those data, when we destroy our data-structure.

### Important Data-Structures and Algorithms

---

#### Helper functions:
```c
// src/dslib/destroy.h
typedef void (*FreeDataFunc)(void *);

// src/dslib/destroy.c
void free_data_func(void *data) {
  if (data != NULL) free(data);
}

// src/dslib/datastr.h
typedef char *(*DataToString)(void *);

// src/dslib/datastr.c
char *char_data_to_string(void *arg) {
  char data = *(char *)arg;
  char *buffer = malloc(50);
  snprintf(buffer, 50, "%c", data);
  return buffer;
}
```

---

#### Stack - `src/dslib/stack.h`
```c
typedef struct StackNode {
  void *data;
  struct StackNode *next;
} StackNode;

typedef struct {
  StackNode *top;
  size_t size;
  pthread_rwlock_t rwlock;
} Stack;

Stack *stack_create(void);
void stack_push(Stack *stack, void *data);
void *stack_pop(Stack *stack, FreeDataFunc freedatafunc);
void *stack_peek(Stack *stack);
void *stack_get(Stack *stack, size_t position);
void stack_print(Stack *stack, DataToString tostring);
void stack_destroy(Stack *stack, FreeDataFunc freedatafunc);
```

Stack is a simple data-structure, we push items on the top and also remove items from the top. The item which goes in first comes out at last. Stack help in DFS (depth first search) in trees and graphs, i.e. we move to the last node first and then move to the top.

In recursion case, it help to go to the base condition and then accumulate the result moving up.

---

#### Queue - `src/dslib/queue.h`

```c
typedef struct QueueNode {
  void *data;
  struct QueueNode *next;
} QueueNode;

typedef struct {
  QueueNode *start;
  QueueNode *end;
  size_t size;
  pthread_rwlock_t rwlock;
} Queue;

Queue *queue_create(void);
void queue_enqueue(Queue *queue, void *data);
void *queue_dequeue(Queue *queue, FreeDataFunc freedatafunc);
void *queue_peek(Queue *queue);
void *queue_get(Queue *queue, size_t position);
void queue_print(Queue *queue, DataToString tostring);
void queue_destroy(Queue *queue, FreeDataFunc freedatafunc);

```
Queue stores the data at the end and gives from the start. The items which goes in first comes out first. This helps in BFS (breadth first search) in trees and graphs. We visit the nearest nodes first before moving to the farthest nodes, opposite to the Stack.

---

#### HashMap - `src/dslib/hashmap.h`

```c
typedef struct HashNode {
  char *key;
  void *value;
  struct HashNode *next;  // seperate chining in linked list
} HashNode;

typedef struct {
  size_t size;
  HashNode **buckets;  // Array of pointers to linked lists (HashNode)
  pthread_rwlock_t rwlock;
} HashMap;

HashMap *hashmap_create(size_t size);
void hashmap_put(HashMap *map, char *key, void *value);
void *hashmap_get(HashMap *map, char *key);
void hashmap_delete(HashMap *map, char *key, FreeDataFunc freedatafunc);
void hashmap_print(HashMap *map, DataToString tostring);
void hashmap_destroy(HashMap *map, FreeDataFunc freedatafunc);

```
HashMap stores key-value pairs. The hash function maps the key to an index. In case of collision, the values are moved to a seperate chain (linkedlist). The primary use here is to search a value using a key in O(1) time in the best case. 

We will use the HashMap to store the Maze values and their respective Graph Node Ids. It will also be used to store pair waise distance, to speed up the miminum distance calucation.

---

#### LinkedList - `src/dslib/list.h`

```c
typedef bool (*ListMatcher)(void *item, void *match);

typedef struct ListNode {
  void *data;
  struct ListNode *next;
} ListNode;

typedef struct {
  ListNode *tail;  // keeps the insertion order
  ListNode *head;
  size_t size;
  pthread_rwlock_t rwlock;
} List;

List *list_create(void);
size_t list_add(List *list, void *data);
size_t list_add_at(List *list, void *data, size_t index);
void *list_delete_at(List *list, size_t index, FreeDataFunc freedatafunc);
void *list_get_at(List *list, size_t index);
long list_index_of(List *list, void *match, ListMatcher matcher);
void list_print(List *list, DataToString tostring);
void list_destroy(List *list, FreeDataFunc freedatafunc);

```

LinkedList stores the data in a sequence. It allows to access or remove the data at a particular position. We will use the list to store the isolated nodes while building the graph. Also, we will use it to store the checkpoints while distance calculation.

---

#### Tree - `src/dslib/tree.h`

```c
typedef struct TreeNode {
  size_t id;
  void *data;
  size_t csize;
  struct TreeNode **children;
  struct TreeNode *parent;
} TreeNode;

typedef struct {
  bool debug;
  size_t counter;
  DataToString tostring;
} TreeCallbackArg;

typedef void (*TreeCallback)(TreeNode *, TreeCallbackArg *arg);

typedef struct {
  bool debug;
  size_t size;
  TreeNode *root;
  pthread_rwlock_t rwlock;
} Tree;

Tree *tree_create(void);
TreeNode *tree_find_dfs(Tree *tree, size_t nodeid);
TreeNode *tree_find_bfs(Tree *tree, size_t nodeid);
void *tree_get(Tree *tree, size_t nodeid);
size_t tree_insert_root(Tree *tree, void *data);
TreeNode *tree_insert_node(Tree *tree, void *data, TreeNode *parent);
size_t tree_insert(Tree *tree, void *data, size_t parentid);
size_t tree_delete(Tree *tree, size_t nodeid, FreeDataFunc freedatafunc);
size_t tree_max_depth(Tree *tree);
void tree_print_raw(Tree *tree, DataToString tostring);
void tree_print(Tree *tree, DataToString tostring);
void tree_destroy(Tree *tree, FreeDataFunc freedatafunc);

```
Tree stores the data in hierarchy. Each child has a single parent and a parent can have multiple children. We will use tree to experiment with an alternate way to find the minimum distance. But it will be very memory hungry as the distance between the points on maze increases. 

---

#### Graph - `src/dslib/graph.h`

```c
// Traversal direction will be opposite to the direction of visit
typedef struct {
  size_t weight;
  struct GraphNode *end;  // Use forward-declared GraphNode
} GraphEdge;

typedef struct GraphNode {
  size_t id;
  void *data;
  size_t esize;
  GraphEdge **edges;
} GraphNode;

typedef struct {
  bool debug;
  size_t counter;
  DataToString tostring;
} GraphCallbackArg;

typedef void (*GraphCallback)(GraphNode *, GraphCallbackArg *arg);
typedef void (*GraphDataCallback)(void *data);
typedef void *(*GraphDataCopier)(void *data);

/**
 * Think as if a 1D array of nodes, in which a node can connect with other nodes via connections
 */
typedef struct {
  bool debug;
  size_t size;
  List *inodes;  // isolated nodes
  GraphNode *root;
  pthread_rwlock_t rwlock;
} Graph;

Graph *graph_create(void);
GraphNode *graph_find_bfs(Graph *graph, size_t nodeid);
GraphNode *graph_find_dfs(Graph *graph, size_t nodeid);
void *graph_get(Graph *graph, size_t nodeid);
// var arg for node ids to link with other nodes
size_t graph_insert(Graph *graph, void *data, size_t linkcount, ...);
size_t graph_insert_arr(Graph *graph, void *data, size_t linkcount, size_t *nodeids);
size_t graph_delete(Graph *graph, size_t nodeid, FreeDataFunc freedatafunc);
void graph_print(Graph *graph, DataToString tostring);
void graph_traverse(Graph *graph, GraphDataCallback callback);
Graph *graph_clone(Graph *graph, GraphDataCopier datacopier);
void graph_destroy(Graph *graph, FreeDataFunc freedatafunc);

```

Graph is very similar to a tree, only that each children can have multiple parents. So, a node can be connected with many other nodes. The connection between nodes are called edges. We will use graph to store maze data and the path. The minimum distance between pair of points will give us the solution of the maze problem.

---

#### Path - `src/dslib/path.h`

```c
// this is used to send the result of the path search, id is the graph id, and data is graph data
typedef struct {
  size_t id;
  size_t cost;
  void *data;
} Location;

Location *location_create(void *data);
Location *location_clone(Location *loc);

void free_location_data_func(void *data);

char *path_tree_data_to_string(void *arg);

// non weighted graph
Stack *path_shortest_nwg_tree_vis(Graph *graph, size_t srcnodeid, size_t dstnodeid,
                                  DataToString tostring);
// non weighted graph
Stack *path_shortest_nwg_tree(Graph *graph, size_t srcnodeid, size_t dstnodeid);

// any graph with search better algorithm
// graph must contain location data
Stack *path_find_shortest(Graph *graph, size_t srcnodeid, size_t dstnodeid);

```

Path implements the algorithms to find the shortest distance using graph (based on Dijkstra algorithm) and various experimental approaches using trees.

---

#### BufferQueue - `src/dslib/bufferq.h`

```c
typedef struct {
  bool debug;
  bool writerclosed;
  bool readerclosed;
  size_t capacity;
  Queue *queue;
  pthread_mutexattr_t mutexattr;
  pthread_mutex_t mutex;
  pthread_cond_t writecond;
  pthread_cond_t readcond;
} BufferQueue;

BufferQueue *bufferq_create(size_t capacity);
void bufferq_write(BufferQueue *bq, void *data);
void *bufferq_read(BufferQueue *bq);
bool bufferq_can_read(BufferQueue *bq);
void bufferq_close_writer(BufferQueue *bq);
void bufferq_destroy(BufferQueue *bq, FreeDataFunc freefunc);

```

BufferQueue data-structure allows to read and write using a fixed capacity queue. When the buffer is empty the reader will wait for more items, and when it is full then the writer will wait for items to be read. 

We will use BufferQueue to process path permutation, using 1 producer thread and multiple consumer threads. This will make the maze solution finding concurrent and fast.

---

### Multithreading

The project has developed few structures to make multithreading simpler (src/crun). We will use `pthreads` to build our solution. 

> A pthread, short for POSIX thread, is a unit of execution within a process in POSIX-compliant operating systems, such as Linux and Unix. The POSIX threads, or pthreads, library provides a standard API for creating and managing threads in a C or C++ program.

---

#### Croutine - `src/crun/croutine.h`

```c
typedef void (*Croutine)(void *context);

typedef struct {
  Croutine croutine;
  void *context;
} Execution;
```

Croutine provides a function type which will be used by a thread for executing a given task.

#### Runtime - `src/crun/runtime.h`

```c
typedef struct {
  char *name;
  Queue *execs;
  pthread_mutexattr_t mutexattr;
  pthread_mutex_t mutex;  // can acquire lock again, which rwlock does not provide
  pthread_cond_t cond;
  pthread_t thread;
  bool exit;
  bool pause;
  bool debug;
} Runtime;

Runtime *runtime_create(void);
void runtime_pause(Runtime *runtime);
void runtime_resume(Runtime *runtime);
void runtime_exec(Runtime *runtime, Croutine croutine, void *context);
void runtime_debug(Runtime *runtime, char *name);
void runtime_join_destroy(Runtime *runtime);

```

Runtime creates a thread and stores a queue of croutines. The thread one by one executes the croutines and then wait for more croutines.

#### Runpool - `src/crun/runpool.h`

```c
typedef struct {
  char *name;
  bool debug;
  size_t size;
  Runtime **runtimes;
} Runpool;

Runpool *runpool_create(size_t size);
void runpool_exec(Runpool *pool, Croutine croutine, void *context);
void runpool_debug(Runpool *pool, char *poolname);
void runpool_join_destroy(Runpool *pool);
```

Runpool creates a pool of threads using Runtimes, and executes the croutines using a thread pool, in a way to maximize their execution. The free threads or thread with minimum load are given the task first.

#### Pipeline - `src/crun/pipeline.h`

```c
typedef void (*PipelineWork)(BufferQueue* bq, void* context);

typedef struct {
  void* context;
  PipelineWork work;
  BufferQueue* bq;
} PipelineContextWrap;

typedef struct {
  bool debug;
  BufferQueue* bq;
  Runpool* prodpool;
  Runpool* conspool;
} Pipeline;

Pipeline* pipeline_create(size_t prodworkers, size_t consworkers,
                          size_t capacity);

void pipeline_add_producer(Pipeline* pipe, PipelineWork work, void* context);
void pipeline_add_consumer(Pipeline* pipe, PipelineWork work, void* context);
void pipeline_debug(Pipeline* pipe);
void pipeline_join_destory(Pipeline* pipe,  FreeDataFunc freedatafunc);

```

Pipeline simplifies the producer-consumer multithreded execution using BufferQueue, and Runpool.

#### Pipeline example - `src/demo/puzzledemo.c`

```c

void path_permutation_producer(BufferQueue *bq, void *context) {
  size_t arrsize = *(size_t *)context;
  int arr[arrsize];

  for (size_t i = 0; i < arrsize; i++) {
    arr[i] = i + 1;
  }

  generate_permutations_buffered(bq, arr, arrsize); // src/puzzle/utils.c
}

void path_permutation_consumer(BufferQueue *bq, void *context) {
  size_t arrsize = *(size_t *)context;

  while (bufferq_can_read(bq)) {
    int *arr = (int *)bufferq_read(bq);
    if (arr == NULL) continue;

    for (size_t i = 0; i < arrsize; i++) {
      printf("%d ", arr[i]);
    }
    printf("\n");
    sleep(2);
  }
}

int path_permutation_pipeline_demo(void) {

  // 1 producer, 3 consumers, 4 work capacity
  Pipeline *pipe = pipeline_create(1, 3, 4);
  pipeline_debug(pipe);

  size_t *arrsize = malloc(sizeof(size_t));
  *arrsize = 4;

  // 1 producer
  pipeline_add_producer(pipe, path_permutation_producer, arrsize);

  // 3 consumers
  pipeline_add_consumer(pipe, path_permutation_consumer, arrsize);
  pipeline_add_consumer(pipe, path_permutation_consumer, arrsize);
  pipeline_add_consumer(pipe, path_permutation_consumer, arrsize);

  pipeline_join_destory(pipe, NULL);
  free(arrsize);

  return EXIT_SUCCESS;
}
```

### Demo
The working examples of every components can be found in `src/demo`. You can execute the demo function using `src/debug/dfunc.h`

```bash
make runclab cmd1=debug cmd2=maze_solution_demo
```

---

## Detailed Discussion on the Maze Problem
[Article - TODO]()


## Project Structure
```
.
├── Makefile
├── README.md
├── bin [generates on make]
│   ├── clab [binary]
│   └── clabdev [binary]
├── build [generates on make]
│   ├── dev [directory]
│   └── release [directory]
└── src
    ├── crun [multithreading]
    │   ├── clocktime.c
    │   ├── clocktime.h
    │   ├── croutine.h
    │   ├── pipeline.c
    │   ├── pipeline.h
    │   ├── runpool.c
    │   ├── runpool.h
    │   ├── runtime.c
    │   └── runtime.h
    ├── demo [working examples]
    │   ├── crundemo.c
    │   ├── crundemo.h
    │   ├── dslibdemo.c
    │   ├── dslibdemo.h
    │   ├── model.c
    │   ├── model.h
    │   ├── pathdemo.c
    │   ├── pathdemo.h
    │   ├── puzzledemo.c
    │   ├── puzzledemo.h
    │   ├── termdemo.c
    │   └── termdemo.h
    ├── dslib [data-structures and algorithms]
    │   ├── bufferq.c
    │   ├── bufferq.h
    │   ├── datastr.c
    │   ├── datastr.h
    │   ├── destroy.c
    │   ├── destroy.h
    │   ├── dump.c
    │   ├── graph.c
    │   ├── graph.h
    │   ├── hashmap.c
    │   ├── hashmap.h
    │   ├── list.c
    │   ├── list.h
    │   ├── path.c
    │   ├── path.h
    │   ├── queue.c
    │   ├── queue.h
    │   ├── stack.c
    │   ├── stack.h
    │   ├── tree.c
    │   ├── tree.h
    │   ├── util.c
    │   └── util.h
    ├── puzzle [maze problem]
    │   ├── maps.txt
    │   ├── maze.c
    │   ├── maze.h
    │   ├── mazesoln.c
    │   ├── util.c
    │   └── util.h
    ├── term [terminal animation]
    │   ├── console.c
    │   └── console.h
    ├── debug [execute single function]
    │   ├── dfunc.c
    │   └── dfunc.h
    └── main.c
    
```

## Using Project
The project has been tested on MacOS and Linux (Ubuntu and Kali Linux).

### Build Binaries
```bash
# To build both dev and release binaries
make

# To build only the dev binary
make dev

# To build only the release binary
make release

```

### Run Dev Program
```bash
make runclabdev cmd1=stack

# or 
make runclabdev cmd1=debug cmd2=maze_solution_demo

# or
./bin/clabdev stack

# or
./bin/clabdev debug maze_solution_demo
```

### Run Release Program
```bash
make runclab cmd1=stack

# or 
make runclab cmd1=debug cmd2=maze_solution_demo

# or
./bin/clab stack

# or
./bin/clab debug maze_solution_demo
```

### To remove all build files and binaries
```bash
make clean
```

## Run the Maze solution
```
make runclab cmd1=debug cmd2=maze_solution

Enter number of rows: 6
Enter number of columns: 28
Enter elements of the 6x28 array:
############################
#@...S...###........@..#####
##...##...@...###..........#
####.#####...........@.#####
#.@.........@...####.....G##
############################
```

You will get the output solution.
```
...
Maze: shortest travel distance 45
Time taken: 0.029095 sec
```

>You can find many maze example in `src/puzzle.maps.txt`. You can copy and paste the maze in the terminal, when array is asked by the program.

>Note: There can be more than one solution path, but the program will show only one out of them.

## Find this project useful ? :heart:
* Support it by clicking the :star: button on the upper right of this page. :v:

## More on YouTube channel - fifocode
Subscribe to the YouTube channel `fifocode` for understanding the concepts used in this project:

[![YouTube](https://img.shields.io/badge/YouTube-Subscribe-red?style=for-the-badge&logo=youtube&logoColor=white)](https://www.youtube.com/@fifocode)

## Contribution
Please feel free to fork it and open a PR (many optimization are possible). You can also add more puzzles in the project.