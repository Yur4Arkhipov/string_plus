#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include "s21_string.h"

#define BUF_SIZE 1024

// smaller buffer for intermediate calculations
#define SBUF_SIZE 256

#define EPSILON 1e-17L

typedef struct settings {
  // Flags
  bool left_justify;     // flag '-'
  bool force_sign;       // flag '+'
  bool force_space;      // flag ' '
  bool sharp;            // flag '#'
  bool left_pad_zeroes;  // flag '0'

  // Width
  bool set_width;
  int width;

  // Precision
  bool set_precision;
  int precision;

  // Length
  bool short_int;    // flag 'h'
  bool long_int;     // flag 'l'
  bool long_double;  // flag 'L'

  // Specifier
  char specifier;
} settings;

void insert_inplace(char* buf, const char* str, s21_size_t start_index);
void to_upper_inplace(char* buf);
bool is_digit(char c);
int read_int(const char** format, int* value);
void read_flags(const char** format, settings* settings);
void read_width(const char** format, settings* settings, va_list ap);
void read_precision(const char** format, settings* settings, va_list ap);
void read_length(const char** format, settings* settings);
void read_specifier(const char** format, settings* settings);
void read_settings(const char** format, settings* settings, va_list ap);
int handle_char(char* buf, const settings* settings, va_list ap);
void handle_int_precision(char** ptmp, const settings* settings, int written);
void int_to_str(char* buf, const settings* settings, long value);
void unsigned_int_to_str(char* buf, settings* settings, unsigned long value,
                         int base);
void handle_int(char* buf, const settings* settings, va_list ap);
void handle_unsigned_int(char* buf, settings* settings, va_list ap, int base);
int double_get_precision(const settings* settings);
int normalize_double(long double* value);
void frac_to_str(char** ptmp, const settings* settings, long double value);
void double_handle_sign(char** ptmp, const settings* settings,
                        long double* value);
void double_to_str(char* buf, const settings* settings, long double value);
void double_to_str_e(char* buf, const settings* settings, long double value);
void remove_trailing_zeroes(char* buf, bool scientific);
void handle_double_shortest(char* buf, const settings* settings,
                            long double value);
void handle_double(char* buf, const settings* settings, va_list ap);
int handle_str(char* buf, const settings* settings, va_list ap);
void handle_prefix(char* buf, const settings* settings);
void handle_pointer(char* buf, settings* settings, va_list ap);
int arg_to_str(char* buf, settings* settings, va_list ap);
void copy_with_width(char** str, const char* buf, const settings* settings);
int s21_sprintf(char* str, const char* format, ...);

void insert_inplace(char* buf, const char* str, s21_size_t start_index) {
  char* new_str = s21_insert(buf, str, start_index);
  int len = s21_strlen(new_str);
  s21_strncpy(buf, new_str, len + 1);
  free(new_str);
}

void to_upper_inplace(char* buf) {
  char* new_str = s21_to_upper(buf);
  int len = s21_strlen(new_str);
  s21_strncpy(buf, new_str, len + 1);
  free(new_str);
}

bool is_digit(char c) { return c >= '0' && c <= '9'; }

// returns 0 if int is present, -1 otherwise (`int* value` is not modified)
// only positive integers
int read_int(const char** format, int* value) {
  int status = 0;
  if (is_digit(**format)) {
    *value = 0;
    while (is_digit(**format)) {
      *value = *value * 10 + (**format - '0');
      (*format)++;
    }
  } else {
    status = -1;
  }
  return status;
}

void read_flags(const char** format, settings* settings) {
  bool loop = true;
  while (**format && loop) {
    switch (**format) {
      case '-':
        settings->left_justify = true;
        break;
      case '+':
        settings->force_sign = true;
        break;
      case ' ':
        settings->force_space = true;
        break;
      case '#':
        settings->sharp = true;
        break;
      case '0':
        settings->left_pad_zeroes = true;
        break;
      default:
        loop = false;
    }
    if (loop) {
      (*format)++;
    }
  }
}

void read_width(const char** format, settings* settings, va_list ap) {
  if (**format == '*') {
    settings->set_width = true;
    settings->width = va_arg(ap, int);
    (*format)++;
  } else {
    int status = read_int(format, &settings->width);
    if (status == 0) {
      settings->set_width = true;
    }
  }
}

void read_precision(const char** format, settings* settings, va_list ap) {
  if (**format == '.') {
    (*format)++;
    settings->set_precision = true;
    if (**format == '*') {
      settings->precision = va_arg(ap, int);
      (*format)++;
    } else {
      settings->precision = 0;
      read_int(format, &settings->precision);
    }
  }
}

void read_length(const char** format, settings* settings) {
  if (**format == 'h') {
    settings->short_int = true;
    (*format)++;
  } else if (**format == 'l') {
    settings->long_int = true;
    (*format)++;
  } else if (**format == 'L') {
    settings->long_double = true;
    (*format)++;
  }
}

void read_specifier(const char** format, settings* settings) {
  settings->specifier = **format;
  (*format)++;
}

void read_settings(const char** format, settings* settings, va_list ap) {
  read_flags(format, settings);
  read_width(format, settings, ap);
  read_precision(format, settings, ap);
  read_length(format, settings);
  read_specifier(format, settings);
}

int handle_char(char* buf, const settings* settings, va_list ap) {
  int status = 0;
  if (!settings->long_int) {
    char value = va_arg(ap, int);
    buf[0] = value;
    buf[1] = 0;
  } else {
    long c = va_arg(ap, long);
    int wstatus = wctomb(buf, c);
    if (wstatus != -1) {
      buf[wstatus] = 0;
    } else {
      status = -1;
    }
  }
  return status;
}

void handle_int_precision(char** ptmp, const settings* settings, int written) {
  int precision = 1;
  if (settings->set_precision) {
    precision = settings->precision;
  }
  while (written < precision) {
    *(*ptmp)-- = '0';
    written++;
  }
}

void int_to_str(char* buf, const settings* settings, long value) {
  char tmp[SBUF_SIZE] = "";
  bool negative = value < 0 ? true : false;
  char* ptmp = tmp + SBUF_SIZE - 2;

  while (value != 0) {
    *ptmp-- = '0' + labs(value % 10);
    value /= 10;
  }
  handle_int_precision(&ptmp, settings, tmp + SBUF_SIZE - 2 - ptmp);

  if (negative) {
    *ptmp-- = '-';
  } else if (settings->force_sign) {
    *ptmp-- = '+';
  } else if (settings->force_space) {
    *ptmp-- = ' ';
  }
  s21_strncpy(buf, ptmp + 1, s21_strlen(ptmp + 1) + 1);
}

void unsigned_int_to_str(char* buf, settings* settings, unsigned long value,
                         int base) {
  char tmp[SBUF_SIZE] = "";
  char* ptmp = tmp + SBUF_SIZE - 2;

  // if value is 0, do not prepend 0x or 0
  if (value == 0) {
    settings->sharp = false;
  }

  while (value != 0) {
    *ptmp-- = "0123456789abcdef"[value % base];
    value /= base;
  }
  handle_int_precision(&ptmp, settings, tmp + SBUF_SIZE - 2 - ptmp);
  s21_strncpy(buf, ptmp + 1, SBUF_SIZE);
}

void handle_int(char* buf, const settings* settings, va_list ap) {
  long value;
  if (settings->short_int) {
    value = (short)va_arg(ap, int);
  } else if (settings->long_int) {
    value = va_arg(ap, long);
  } else {
    value = va_arg(ap, int);
  }
  int_to_str(buf, settings, value);
}

void handle_unsigned_int(char* buf, settings* settings, va_list ap, int base) {
  unsigned long value;
  if (settings->short_int) {
    value = (unsigned short)va_arg(ap, unsigned int);
  } else if (settings->long_int) {
    value = va_arg(ap, unsigned long);
  } else {
    value = va_arg(ap, unsigned int);
  }
  unsigned_int_to_str(buf, settings, value, base);
}

int double_get_precision(const settings* settings) {
  int precision = 6;
  if (settings->set_precision) {
    precision = settings->precision;
  }
  return precision;
}

int normalize_double(long double* value) {
  int exp = 0;
  if (fabsl(*value) > EPSILON) {
    while (*value > 10) {
      *value /= 10;
      exp++;
    }
    while (*value < 1) {
      *value *= 10;
      exp--;
    }
  }
  return exp;
}

void frac_to_str(char** ptmp, const settings* settings, long double value) {
  int precision = double_get_precision(settings);
  for (int i = 0; i < precision; i++) {
    value *= 10;
    int digit = (int)value;
    *(*ptmp)++ = digit + '0';
    value -= digit;
  }
  **ptmp = 0;
}

void double_handle_sign(char** ptmp, const settings* settings,
                        long double* value) {
  if (signbit(*value)) {
    *(*ptmp)++ = '-';
    *value = -*value;
  } else if (settings->force_sign) {
    *(*ptmp)++ = '+';
  } else if (settings->force_space) {
    *(*ptmp)++ = ' ';
  }
}

void double_to_str(char* buf, const settings* settings, long double value) {
  char tmp[SBUF_SIZE];
  char* ptmp = tmp;

  double_handle_sign(&ptmp, settings, &value);

  int precision = double_get_precision(settings);
  // add 5 to the right of the last digit (rounding)
  value += 5 * powl(10, -precision - 1);

  long v = value;
  struct settings int_settings = {0};
  int_to_str(ptmp, &int_settings, v);
  ptmp += s21_strlen(ptmp);
  value -= (long double)v;
  if (precision > 0) {
    *ptmp++ = '.';
  }
  frac_to_str(&ptmp, settings, value);

  *ptmp = 0;
  s21_strncpy(buf, tmp, SBUF_SIZE);
}

void double_to_str_e(char* buf, const settings* settings, long double value) {
  char tmp[SBUF_SIZE];
  char* ptmp = tmp;

  double_handle_sign(&ptmp, settings, &value);

  int exp = normalize_double(&value);

  int precision = double_get_precision(settings);
  if (fabsl(value) > EPSILON) {
    value += 5 * powl(10, -precision - 1);
  }
  exp += normalize_double(&value);
  int digit = value;
  *ptmp++ = '0' + digit;
  value -= digit;
  if (precision > 0) {
    *ptmp++ = '.';
  }

  frac_to_str(&ptmp, settings, value);
  *ptmp++ = 'e';

  struct settings int_settings = {
      .set_precision = true, .precision = 2, .force_sign = true};
  int_to_str(ptmp, &int_settings, exp);
  s21_strncpy(buf, tmp, SBUF_SIZE);
}

void remove_trailing_zeroes(char* buf, bool scientific) {
  buf += s21_strlen(buf);
  if (!scientific) {
    if (buf[-1] == '0') {
      while (*--buf == '0') {
        *buf = 0;
      }
      if (*buf == '.') {
        *buf = 0;
      }
    }
  } else {
    char exp[10];
    buf -= 4;
    s21_strncpy(exp, buf, 5);
    if (buf[-1] == '0') {
      while (*--buf == '0') {
        *buf = 0;
      }
      if (*buf == '.') {
        *buf = 0;
      }
      s21_strncpy(buf, exp, 5);
    }
  }
}

void handle_double_shortest(char* buf, const settings* settings,
                            long double value) {
  int precision = double_get_precision(settings);
  precision += precision == 0 ? 1 : 0;
  long double tmp = value;
  int exp = normalize_double(&tmp);
  if (fabsl(value) > EPSILON) {
    tmp += 5 * powl(10, -precision - 1);
  }
  exp += normalize_double(&tmp);
  // https://stackoverflow.com/questions/54162152/what-precisely-does-the-g-printf-specifier-mean
  if (exp >= -4 && exp < precision) {
    struct settings custom_settings = *settings;
    custom_settings.set_precision = true;
    custom_settings.precision = precision - 1 - exp;
    double_to_str(buf, &custom_settings, value);
    remove_trailing_zeroes(buf, false);
  } else {
    struct settings custom_settings = *settings;
    custom_settings.set_precision = true;
    custom_settings.precision = precision - 1;
    double_to_str_e(buf, &custom_settings, value);
    remove_trailing_zeroes(buf, true);
  }
}

void handle_double(char* buf, const settings* settings, va_list ap) {
  long double value;
  if (settings->long_double) {
    value = va_arg(ap, long double);
  } else {
    value = va_arg(ap, double);
  }

  switch (settings->specifier) {
    case 'f':
      double_to_str(buf, settings, value);
      break;
    case 'e':
    case 'E':
      double_to_str_e(buf, settings, value);
      break;
    case 'g':
    case 'G':
      handle_double_shortest(buf, settings, value);
      break;
  }
}

int handle_str(char* buf, const settings* settings, va_list ap) {
  int status = 0;
  int precision = BUF_SIZE - 2;
  if (settings->set_precision) {
    precision = settings->precision;
  }
  if (!settings->long_int) {
    const char* s = va_arg(ap, char*);
    s21_strncpy(buf, s, precision);
    buf[precision] = 0;
  } else {
    const wchar_t* s = va_arg(ap, wchar_t*);
    int wstatus = wcstombs(buf, s, precision);
    if (wstatus == -1) {
      status = -1;
    }
  }
  return status;
}

void handle_prefix(char* buf, const settings* settings) {
  if (settings->sharp) {
    switch (settings->specifier) {
      case 'o':
        insert_inplace(buf, "0", 0);
        break;
      case 'x':
      case 'p':
      case 'X':
        insert_inplace(buf, "0x", 0);
        break;
    }
  }
}

void handle_pointer(char* buf, settings* settings, va_list ap) {
  struct settings custom_settings = *settings;
  custom_settings.sharp = true;
  unsigned long ptr = va_arg(ap, unsigned long);
  if (ptr == 0) {
    s21_strncpy(buf, "(nil)", 6);
  } else {
    unsigned_int_to_str(buf, settings, ptr, 16);
    handle_prefix(buf, &custom_settings);
  }
}

int arg_to_str(char* buf, settings* settings, va_list ap) {
  int status = 0;
  switch (settings->specifier) {
    case 'c':
      status = handle_char(buf, settings, ap);
      break;
    case 'd':
    case 'i':
      handle_int(buf, settings, ap);
      break;
    case 'e':
    case 'f':
    case 'g':
      handle_double(buf, settings, ap);
      break;
    case 'E':
    case 'G':
      handle_double(buf, settings, ap);
      to_upper_inplace(buf);
      break;
    case 'o':
      handle_unsigned_int(buf, settings, ap, 8);
      handle_prefix(buf, settings);
      break;
    case 's':
      status = handle_str(buf, settings, ap);
      break;
    case 'u':
      handle_unsigned_int(buf, settings, ap, 10);
      break;
    case 'x':
      handle_unsigned_int(buf, settings, ap, 16);
      handle_prefix(buf, settings);
      break;
    case 'X':
      handle_unsigned_int(buf, settings, ap, 16);
      handle_prefix(buf, settings);
      to_upper_inplace(buf);
      break;
    case 'p':
      handle_pointer(buf, settings, ap);
      break;
    case '%':
      buf[0] = '%';
      buf[1] = 0;
      break;
  }
  return status;
}

void copy_with_width(char** str, const char* buf, const settings* settings) {
  int len = s21_strlen(buf);
  char c = settings->left_pad_zeroes ? '0' : ' ';
  int remaining = settings->set_width ? settings->width - len : 0;
  if (settings->left_justify) {
    s21_strncpy(*str, buf, len + 1);
    *str += len;
    while (remaining-- > 0) {
      *(*str)++ = c;
    }
  } else {
    while (remaining-- > 0) {
      *(*str)++ = c;
    }
    s21_strncpy(*str, buf, len + 1);
    *str += len;
  }
  **str = 0;
}

int s21_sprintf(char* str, const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  const char* str_start = str;
  int err = 0;
  while (*format && !err) {
    if (*format != '%') {
      *str++ = *format++;
      continue;
    } else {
      format++;
      if (*format == 'n') {
        int* ptr = va_arg(ap, int*);
        *ptr = str - str_start;
        format++;
        continue;
      }
      settings settings = {0};
      read_settings(&format, &settings, ap);
      char buf[BUF_SIZE];
      int status = arg_to_str(buf, &settings, ap);
      if (status == 0) {
        copy_with_width(&str, buf, &settings);
      } else {
        err = -1;
      }
    }
  }
  va_end(ap);
  *str = 0;
  int ret = str - str_start;
  if (err) {
    ret = -1;
  }
  return ret;
}
