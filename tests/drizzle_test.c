/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

#include "config.h"

#if defined(NDEBUG)
# undef NDEBUG
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libgearman/gearman.h>

#include "libtest/test.h"
#include "libtest/server.h"

#define WORKER_TEST_PORT 32123
#define WORKER_FUNCTION "drizzle_queue_test"

typedef struct
{
  pid_t gearmand_pid;
  gearman_worker_st worker;
  bool run_worker;
} worker_test_st;

/* Prototypes */
test_return_t queue_add(void *object);
test_return_t queue_worker(void *object);

test_return_t pre(void *object);
test_return_t post(void *object);
test_return_t flush(void *object);

void *world_create(test_return_t *error);
test_return_t world_destroy(void *object);

static void client_logger(const char *line, gearman_verbose_t verbose, void *context)
{
  (void)line;
  (void)context;
  (void)verbose;
  //fprintf(stderr, "\nclient_logger: %s\n", line);
}

/* Counter test for worker */
static void *counter_function(gearman_job_st *job __attribute__((unused)),
                              void *context, size_t *result_size,
                              gearman_return_t *ret_ptr __attribute__((unused)))
{
  uint32_t *counter= (uint32_t *)context;

  *result_size= 0;

  *counter= *counter + 1;

  return NULL;
}

test_return_t queue_add(void *object)
{
  gearman_return_t rc;
  worker_test_st *test= (worker_test_st *)object;
  gearman_client_st client;
  gearman_client_st *check;
  char job_handle[GEARMAN_JOB_HANDLE_SIZE];
  const char *workload= "set workload";

  test->run_worker= false;

  check= gearman_client_create(&client);
  test_truth(check);

  gearman_client_set_log_fn(&client, client_logger, NULL, GEARMAN_VERBOSE_DEBUG);

  rc= gearman_client_add_server(&client, NULL, WORKER_TEST_PORT);
  test_truth(rc == GEARMAN_SUCCESS);

  rc= gearman_client_do_background(&client, WORKER_FUNCTION, NULL,
                                   workload, sizeof(workload)-1,
                                   job_handle);
  test_true_got(rc == GEARMAN_SUCCESS, gearman_strerror(rc));

  sleep(3); // Yes, this is sleep().

  gearman_client_free(&client);

  test->run_worker= true;
  return TEST_SUCCESS;
}

test_return_t queue_worker(void *object)
{
  worker_test_st *test= (worker_test_st *)object;
  gearman_worker_st *worker= &(test->worker);
  uint32_t counter= 0;

  if (!test->run_worker)
    return TEST_FAILURE;

  if (gearman_worker_add_function(worker, WORKER_FUNCTION, 
                                  5, counter_function,
                                  &counter) != GEARMAN_SUCCESS)
  {
    return TEST_FAILURE;
  }

  if (gearman_worker_work(worker) != GEARMAN_SUCCESS)
    return TEST_FAILURE;

  if (counter == 0)
    return TEST_FAILURE;

  return TEST_SUCCESS;
}

void *world_create(test_return_t *error)
{
  worker_test_st *test;
  pid_t gearmand_pid;
  const char *argv[2]= { "test_gearmand", "" };

  gearmand_pid= test_gearmand_start(WORKER_TEST_PORT, "libdrizzle", (char **)argv, 2);

  test= malloc(sizeof(worker_test_st));
  if (! test)
  {
    *error= TEST_MEMORY_ALLOCATION_FAILURE;
    return NULL;
  }

  memset(test, 0, sizeof(worker_test_st));
  if (gearman_worker_create(&(test->worker)) == NULL)
  {
    *error= TEST_FAILURE;
    return NULL;
  }

  if (gearman_worker_add_server(&(test->worker), NULL, WORKER_TEST_PORT) != GEARMAN_SUCCESS)
  {
    *error= TEST_FAILURE;
    return NULL;
  }

  test->gearmand_pid= gearmand_pid;

  *error= TEST_SUCCESS;

  return (void *)test;
}

test_return_t world_destroy(void *object)
{
  worker_test_st *test= (worker_test_st *)object;
  gearman_worker_free(&(test->worker));
  test_gearmand_stop(test->gearmand_pid);
  free(test);

  return TEST_SUCCESS;
}

test_st tests[] ={
  {"add", 0, queue_add },
  {"worker", 0, queue_worker },
  {0, 0, 0}
};

collection_st collection[] ={
#ifdef HAVE_LIBDRIZZLE
  {"drizzle queue", 0, 0, tests},
#endif
  {0, 0, 0, 0}
};

void get_world(world_st *world)
{
  world->collections= collection;
  world->create= world_create;
  world->destroy= world_destroy;
}