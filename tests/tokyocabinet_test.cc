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

#include <tests/basic.h>
#include <tests/context.h>

#define WORKER_TEST_PORT 32123

void *world_create(test_return_t *error);
test_return_t world_destroy(void *object);

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

static test_return_t collection_init(void *object)
{
  const char *argv[3]= { "test_gearmand", "--libtokyocabinet-file=tests/gearman.tcb", "--queue-type=libtokyocabinet" };

  unlink("tests/gearman.tcb");

  Context *test= (Context *)object;
  assert(test);

  test_truth(test->initialize(3, argv));

  return TEST_SUCCESS;
}

static test_return_t collection_cleanup(void *object)
{
  Context *test= (Context *)object;
  test->reset();

  return TEST_SUCCESS;
}


void *world_create(test_return_t *error)
{
  Context *test= new Context(WORKER_TEST_PORT);
  if (not test)
  {
    *error= TEST_MEMORY_ALLOCATION_FAILURE;
    return NULL;
  }

  *error= TEST_SUCCESS;

  return test;
}

test_return_t world_destroy(void *object)
{
  Context *test= (Context *)object;

  delete test;

  return TEST_SUCCESS;
}

test_st tests[] ={
  {"gearman_client_echo()", 0, client_echo_test },
  {"gearman_client_echo() fail", 0, client_echo_fail_test },
  {"gearman_worker_echo()", 0, worker_echo_test },
  {"clean", 0, queue_clean },
  {"add", 0, queue_add },
  {"worker", 0, queue_worker },
  {0, 0, 0}
};

collection_st collection[] ={
#ifdef HAVE_LIBTOKYOCABINET
  {"tokyocabinet queue", collection_init, collection_cleanup, tests},
#endif
  {0, 0, 0, 0}
};

void get_world(world_st *world)
{
  world->collections= collection;
  world->create= world_create;
  world->destroy= world_destroy;
}
