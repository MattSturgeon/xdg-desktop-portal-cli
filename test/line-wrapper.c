#include "config.h"

#include "../src/line-wrapper.h"

#include <glib.h>
#include <locale.h>

#define A_REALLY_LONG_LINE                                                  \
  "A veryveryverylonglinewith_no_spacesshouldonlywrapifhyphonateisenabled " \
  "don't you think"
#define OVER_ONE_LINE                                                          \
  "This example will probably be around 100 chars long by the time I run out " \
  "of example text to write..."
#define A_STRING_WITH_NEWLINES \
  "\nThis example has a leading newline as well as a trailing newline\n"
#define A_STRING_WITH_HYPHENS \
  "This example has a hyphen in a good-place to wrap the line"

typedef struct {
  gsize    line_len;
  gboolean hyphonate;
  gchar*   input;
  gchar**  output;
  gsize    output_len;
} TestData;

static gchar* copy_string(const gchar* source)
{
  gsize  length = strlen(source);
  gchar* dest   = g_malloc(sizeof(gchar) * (length + 1));
  strncpy(dest, source, length + 1);
  dest[length] = '\0';
  return dest;
}

static TestData build_test_data(gsize    line_len,
                                gboolean hyphonate,
                                gchar*   input,
                                gsize    output_len,
                                gchar*   output[])
{
  TestData test_data;

  test_data.line_len   = line_len;
  test_data.hyphonate  = hyphonate;
  test_data.output_len = output_len;

  test_data.input = copy_string(input);

  test_data.output = g_malloc(sizeof(gchar*) * output_len);
  for (gsize i = 0; i < output_len; i++)
    test_data.output[i] = copy_string(output[i]);

  return test_data;
}

static void free_test_data(const TestData* test_data)
{
  for (gsize i = 0; i < test_data->output_len; i++)
    g_free(test_data->output[i]);

  g_free(test_data->output);
  g_free(test_data->input);
}

static void basic_test_with_data(gconstpointer d)
{
  const TestData* test_data = d;
  g_autoptr(Lines) lines    = NULL;

  lines =
      wrap_lines(test_data->input, test_data->line_len, test_data->hyphonate);

  g_assert_cmpuint(test_data->output_len, ==, lines->len);

  for (gsize i = 0; i < lines->len; i++)
    g_assert_cmpstr(test_data->output[i], ==, lines->array[i]);

  free_test_data(test_data);
}

gint main(gint argc, gchar* argv[])
{
  // Allocate enough memory for 10 tests
  TestData test_data[10];
  gsize    i = 0;

  setlocale(LC_ALL, "");

  g_test_init(&argc, &argv, NULL);

  test_data[i] = build_test_data(strlen(OVER_ONE_LINE),
                                 FALSE,
                                 OVER_ONE_LINE,
                                 1,
                                 (char*[]){
                                     OVER_ONE_LINE,
                                 });
  g_test_add_data_func(
      "/line-wrapper/test-no-break", &test_data[i], basic_test_with_data);

  test_data[++i] = build_test_data(
      80,
      FALSE,
      OVER_ONE_LINE,
      2,
      (char*[]){
          "This example will probably be around 100 chars long by the time I "
          "run out of",
          "example text to write...",
      });
  g_test_add_data_func("/line-wrapper/test-break-before-80",
                       &test_data[i],
                       basic_test_with_data);

  test_data[++i] = build_test_data(20,
                                   FALSE,
                                   OVER_ONE_LINE,
                                   6,
                                   (char*[]){
                                       "This example will",
                                       "probably be around",
                                       "100 chars long by",
                                       "the time I run out",
                                       "of example text to",
                                       "write...",
                                   });
  g_test_add_data_func("/line-wrapper/test-break-before-20",
                       &test_data[i],
                       basic_test_with_data);

  test_data[++i] =
      build_test_data(20,
                      FALSE,
                      A_REALLY_LONG_LINE,
                      3,
                      (char*[]){
                          "A",
                          "veryveryverylonglinewith_no_"
                          "spacesshouldonlywrapifhyphonateisenabled",
                          "don't you think",
                      });
  g_test_add_data_func(
      "/line-wrapper/test-dont-hyphonate", &test_data[i], basic_test_with_data);

  test_data[++i] = build_test_data(20,
                                   TRUE,
                                   A_REALLY_LONG_LINE,
                                   6,
                                   (char*[]){
                                       "A",
                                       "veryveryverylonglin-",
                                       "ewith_no_spacesshou-",
                                       "ldonlywrapifhyphona-",
                                       "teisenabled don't",
                                       "you think",
                                   });
  g_test_add_data_func(
      "/line-wrapper/test-do-hyphonate", &test_data[i], basic_test_with_data);

  test_data[++i] = build_test_data(20,
                                   TRUE,
                                   A_STRING_WITH_NEWLINES,
                                   6,
                                   (char*[]){
                                       "",
                                       "This example has a",
                                       "leading newline as",
                                       "well as a trailing",
                                       "newline",
                                       "",
                                   });
  g_test_add_data_func("/line-wrapper/test-handle-newlines",
                       &test_data[i],
                       basic_test_with_data);

  test_data[++i] = build_test_data(40,
                                   TRUE,
                                   A_STRING_WITH_HYPHENS,
                                   2,
                                   (char*[]){
                                       "This example has a hyphen in a good-",
                                       "place to wrap the line",
                                   });
  g_test_add_data_func(
      "/line-wrapper/test-handle-hyphens", &test_data[i], basic_test_with_data);

  return g_test_run();
}