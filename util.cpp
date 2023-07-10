#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <string>

void error(const char* _message){
  fprintf(stderr, _message, NULL);
  exit(-1);
};
void error_at(const char* _raw_input, char *_location, const char *_fmt, ...){
  va_list ap;
  va_start(ap, _fmt);

  int pos = _location - _raw_input;
  fprintf(stderr, "%s\n", _raw_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^");
  vfprintf(stderr, _fmt, ap);
  fprintf(stderr, "\n");

  exit(-1);
};