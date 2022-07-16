#pragma once

#include <glib.h>

/* The wrapped lines returned by wrap_lines */
typedef struct {
  gsize   len;
  gsize   allocated;
  gchar** array;
} Lines;

/* wrap input to the given line_length. Each line is wrapped on the last
 * whitespace before reaching line_length.
 *
 * If hyphons_if_wrap_impossible is TRUE, hyphons will be inserted if a line is
 * longer than line_length and has no whitespace to wrap on.
 *
 * If hyphons_if_wrap_impossible is FALSE, lines longer than line_length will
 * only be wrapped if they contain whitespace.
 *
 * Returns a pointer to a Lines struct, which should be freed using
 * free_wrapped_lines()
 */
Lines* wrap_lines(const gchar*   input,
                  const gsize    line_length,
                  const gboolean hyphons_if_wrap_impossible);

/*
 * Completely frees the Lines struct.
 *
 * Evey line will be freed, along with the struct and array.
 */
void free_lines(Lines* lines);

/* Enable use of g_autoptr(Lines) */
G_DEFINE_AUTOPTR_CLEANUP_FUNC(Lines, free_lines);