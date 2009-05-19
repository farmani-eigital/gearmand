/* Gearman server and library
 * Copyright (C) 2008 Brian Aker, Eric Day
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/**
 * @file
 * @brief libdrizzle Queue Storage Declarations
 */

#ifndef __GEARMAN_QUEUE_LIBDRIZZLE_H__
#define __GEARMAN_QUEUE_LIBDRIZZLE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup gearman_queue_libdrizzle libdrizzle Queue Storage Functions
 * @ingroup gearman_queue
 * @{
 */

/**
 * Print usage information.
 */
void gearman_queue_libdrizzle_usage(void);

/**
 * Initialize the queue.
 */
gearman_return_t gearman_queue_libdrizzle_init(gearman_st *gearman, int argc,
                                               char *argv[]);

/**
 * De-initialize the queue.
 */
gearman_return_t gearman_queue_libdrizzle_deinit(gearman_st *gearman);

/**
 * Initialize the queue for a gearmand object.
 */
gearman_return_t gearmand_queue_libdrizzle_init(gearmand_st *gearmand,
                                                int argc, char *argv[]);

/**
 * De-initialize the queue for a gearmand object.
 */
gearman_return_t gearmand_queue_libdrizzle_deinit(gearmand_st *gearmand);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __GEARMAN_QUEUE_LIBDRIZZLE_H__ */