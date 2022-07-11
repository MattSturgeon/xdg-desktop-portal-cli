#include "line-wrapper.h"

#include <string.h>

// A small struct to keep track of where we might wrap the line
typedef struct {
  const gchar* wrap_point;
  gboolean     replace;
} wrap_candidate;

gchar* wrap_line(const gchar* start, const gsize length, const gchar* suffix)
{
  gsize  suffix_len = strlen(suffix);
  gsize  input_len  = length - suffix_len;
  gchar* buffer     = g_malloc((length + 1) * sizeof(gchar));

  if (input_len > 0)
    strncpy(buffer, start, input_len);

  if (suffix_len > 0)
    strncpy(buffer + input_len, suffix, suffix_len);

  buffer[length] = '\0';

  return buffer;
}

// Update the most recent char that we could wrap on...
void check_and_update_wrappable_char(wrap_candidate* candidate,
                                     const gchar*    input,
                                     gsize           index)
{
  // First check if we can wrap after the previous char
  if (index > 0) {
    switch (input[index - 1]) {
      case '-':
        {
          candidate->wrap_point = &input[index];
          candidate->replace    = FALSE;
        }
        break;
    }
  }

  // Then check if we can wrap on the current char
  switch (input[index]) {
    // TODO add more whitespace?
    case '\0':
    case '\n':
    case ' ':
      {
        candidate->wrap_point = &input[index];
        candidate->replace    = TRUE;
      }
      break;
  }
}

void realloc_lines(Lines* lines)
{
  gsize new_size   = sizeof(gchar*) * (lines->len + 4);
  lines->array     = g_realloc(lines->array, new_size);
  lines->allocated = new_size;
}

void add_line(Lines* lines, gchar* line)
{
  gsize required_size = sizeof(gchar*) * (lines->len + 1);
  if (required_size > lines->allocated)
    realloc_lines(lines);

  lines->array[lines->len++] = line;
}

Lines* wrap_lines(const gchar*   input,
                  const gsize    line_length,
                  const gboolean hyphons_if_wrap_impossible)
{
  Lines*         lines     = NULL;
  wrap_candidate candidate = {NULL, FALSE};
  const gchar*   last_wrapped_at;
  gsize          i = 0;

  // Initialise lines struct
  lines            = g_malloc(sizeof(Lines));
  lines->len       = 0;
  lines->allocated = 0;
  lines->array     = NULL;
  realloc_lines(lines);

  last_wrapped_at = input;

  do {
    gsize current_line_length = &input[i] - last_wrapped_at;

    // Keep track of the most recent char that we could wrap on
    check_and_update_wrappable_char(&candidate, input, i);

    if (!input[i] || input[i] == '\n' || current_line_length >= line_length) {
      const gchar* wrap_at;
      gchar*       suffix     = "";
      gsize        suffix_len = 0;

      wrap_at = candidate.wrap_point;
      if (wrap_at < last_wrapped_at) {
        // No wrap candidates since last wrap...
        if (hyphons_if_wrap_impossible) {
          suffix  = "-";
          wrap_at = &input[i];
        } else
          continue;
      }

      // Copy the wrapped line to the output array
      add_line(lines,
               wrap_line(last_wrapped_at, wrap_at - last_wrapped_at, suffix));

      // Update last_wrapped_at pointer, then correct for any replaced chars
      last_wrapped_at = wrap_at;

      suffix_len = strlen(suffix);
      if (suffix_len > 0) {
        // Move pointer to where the suffix was inserted
        last_wrapped_at -= suffix_len;
      } else if (candidate.replace) {
        // Move pointer past the whitespace char we just wrapped on
        last_wrapped_at++;
      }
    }
  } while (input[i++]);

  return lines;
}

void free_lines(Lines* wrapped_lines)
{
  if (!wrapped_lines)
    return;

  // Free each pointer
  for (gsize i = 0; i < wrapped_lines->len; i++)
    g_free(wrapped_lines->array[i]);

  g_free(wrapped_lines->array);
  g_free(wrapped_lines);
}
