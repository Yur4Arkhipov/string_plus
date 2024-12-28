#include "s21_string.h"

#include <stdlib.h>

void* s21_memchr(const void* str, int c, s21_size_t n) {
  const unsigned char* s = (const unsigned char*)str;
  unsigned char ch = (unsigned char)c;
  void* result = S21_NULL;

  for (s21_size_t i = 0; i < n; i++) {
    if (s[i] == ch) {
      result = (void*)(s + i);
      break;
    }
  }

  return result;
}
/*
- <0, если первый отличающийся байт (переинтерпретируемый как unsigned char) в
левой части меньше соответствующего байта в правой части.
-​ 0​, если все счётные байты левого и
правого полей равны.
- >0, если первый отличающийся байт в левой части больше соответствующего байта
в правой части.
*/
int s21_memcmp(const void* str1, const void* str2, s21_size_t n) {
  const unsigned char* s1 = (const unsigned char*)str1;
  const unsigned char* s2 = (const unsigned char*)str2;
  int result = 0;

  for (s21_size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i]) {
      result = s1[i] - s2[i];
      break;
    }
  }

  return result;
}

// TODO: check with passing null
void* s21_memcpy(void* dest, const void* src, s21_size_t n) {
  const unsigned char* s = (const unsigned char*)src;
  unsigned char* d = (unsigned char*)dest;

  for (s21_size_t i = 0; i < n; i++) {
    d[i] = s[i];
  }

  return dest;
}

// TODO: check with passing null
void* s21_memset(void* str, int c, s21_size_t n) {
  unsigned char* s = (unsigned char*)str;

  for (s21_size_t i = 0; i < n; i++) {
    s[i] = (unsigned char)c;
  }

  return str;
}

char* s21_strncat(char* dest, const char* src, s21_size_t n) {
  char* end_dest_ptr = dest + s21_strlen(dest);

  s21_size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++) {
    end_dest_ptr[i] = src[i];
  }
  end_dest_ptr[i] = '\0';

  return dest;
}

char* s21_strchr(const char* str, int c) {
  const unsigned char ch = (unsigned char)c;
  int i = 0;

  while (str[i] && str[i] != ch) {
    i++;
  }

  return ch == str[i] ? (char*)(str + i) : S21_NULL;
}

int s21_strncmp(const char* str1, const char* str2, s21_size_t n) {
  const unsigned char* s1 = (const unsigned char*)str1;
  const unsigned char* s2 = (const unsigned char*)str2;
  int result = 0;

  for (s21_size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i]) {
      result = s1[i] - s2[i];
      break;
    }

    // прекращение сравнение при конце строки
    if (s1[i] == '\0' || s2[i] == '\0') {
      break;
    }
  }

  return result;
}

// TODO: check fun tests
char* s21_strncpy(char* dest, const char* src, s21_size_t n) {
  s21_size_t i = 0;
  while (i < n && src[i] != '\0') {
    dest[i] = (char)src[i];
    i++;
  }

  // заполнение нулями если src.count < n
  while (i < n) {
    dest[i] = '\0';
    i++;
  }

  return dest;
}

/* Длина первой части строки string1 не содержащей никакие символы строки
string2. Длина строки string1, если ни один из символов строки string2 не входит
в состав string1. */
s21_size_t s21_strcspn(const char* str1, const char* str2) {
  const char* ptr_str1 = str1;
  char ch_str1;
  s21_size_t result = 0;
  int found = 0;

  while (!found && (ch_str1 = *ptr_str1++) != '\0') {
    const char* ptr_str2 = str2;
    char ch_str2;
    while ((ch_str2 = *ptr_str2++) != '\0') {
      if (ch_str1 == ch_str2) {
        found = 1;
        break;
      }
    }

    if (!found) {
      result++;
    }
  }

  return result;
}

s21_size_t s21_strlen(const char* str) {
  s21_size_t len = 0;

  while (str[len] != '\0') {
    len++;
  }

  return len;
}

#if defined(__APPLE__)
#define MAX_ERRLIST 106
#define ERROR "Unknown error: "

static const char* errorList[] = {
    "Undefined error: 0",
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "Input/output error",
    "Device not configured",
    "Argument list too long",
    "Exec format error",
    "Bad file descriptor",
    "No child processes",
    "Resource deadlock avoided",
    "Cannot allocate memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Resource busy",
    "File exists",
    "Cross-device link",
    "Operation not supported by device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "Too many open files in system",
    "Too many open files",
    "Inappropriate ioctl for device",
    "Text file busy",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Numerical argument out of domain",
    "Result too large",
    "Resource temporarily unavailable",
    "Operation now in progress",
    "Operation already in progress",
    "Socket operation on non-socket",
    "Destination address required",
    "Message too long",
    "Protocol wrong type for socket",
    "Protocol not available",
    "Protocol not supported",
    "Socket type not supported",
    "Operation not supported",
    "Protocol family not supported",
    "Address family not supported by protocol family",
    "Address already in use",
    "Can't assign requested address",
    "Network is down",
    "Network is unreachable",
    "Network dropped connection on reset",
    "Software caused connection abort",
    "Connection reset by peer",
    "No buffer space available",
    "Socket is already connected",
    "Socket is not connected",
    "Can't send after socket shutdown",
    "Too many references: can't splice",
    "Operation timed out",
    "Connection refused",
    "Too many levels of symbolic links",
    "File name too long",
    "Host is down",
    "No route to host",
    "Directory not empty",
    "Too many processes",
    "Too many users",
    "Disc quota exceeded",
    "Stale NFS file handle",
    "Too many levels of remote in path",
    "RPC struct is bad",
    "RPC version wrong",
    "RPC prog. not avail",
    "Program version wrong",
    "Bad procedure for program",
    "No locks available",
    "Function not implemented",
    "Inappropriate file type or format",
    "Authentication error",
    "Need authenticator",
    "Device power is off",
    "Device error",
    "Value too large to be stored in data type",
    "Bad executable (or shared library)",
    "Bad CPU type in executable",
    "Shared library version mismatch",
    "Malformed Mach-o file",
    "Operation canceled",
    "Identifier removed",
    "No message of desired type",
    "Illegal byte sequence",
    "Attribute not found",
    "Bad message",
    "EMULTIHOP (Reserved)",
    "No message available on STREAM",
    "ENOLINK (Reserved)",
    "No STREAM resources",
    "Not a STREAM",
    "Protocol error",
    "STREAM ioctl timeout",
    "Operation not supported on socket",
    "Policy not found",
    "State not recoverable",
    "Previous owner died",
    "Interface output queue is full"};

#elif defined(__linux__)
#define MAX_ERRLIST 133
#define ERROR "Unknown error "

static const char* errorList[] = {
    "Success",
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "Input/output error",
    "No such device or address",
    "Argument list too long",
    "Exec format error",
    "Bad file descriptor",
    "No child processes",
    "Resource temporarily unavailable",
    "Cannot allocate memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Device or resource busy",
    "File exists",
    "Invalid cross-device link",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "Too many open files in system",
    "Too many open files",
    "Inappropriate ioctl for device",
    "Text file busy",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Numerical argument out of domain",
    "Numerical result out of range",
    "Resource deadlock avoided",
    "File name too long",
    "No locks available",
    "Function not implemented",
    "Directory not empty",
    "Too many levels of symbolic links",
    "Unknown error 41",
    "No message of desired type",
    "Identifier removed",
    "Channel number out of range",
    "Level 2 not synchronized",
    "Level 3 halted",
    "Level 3 reset",
    "Link number out of range",
    "Protocol driver not attached",
    "No CSI structure available",
    "Level 2 halted",
    "Invalid exchange",
    "Invalid request descriptor",
    "Exchange full",
    "No anode",
    "Invalid request code",
    "Invalid slot",
    "Unknown error 58",
    "Bad font file format",
    "Device not a stream",
    "No data available",
    "Timer expired",
    "Out of streams resources",
    "Machine is not on the network",
    "Package not installed",
    "Object is remote",
    "Link has been severed",
    "Advertise error",
    "Srmount error",
    "Communication error on send",
    "Protocol error",
    "Multihop attempted",
    "RFS specific error",
    "Bad message",
    "Value too large for defined data type",
    "Name not unique on network",
    "File descriptor in bad state",
    "Remote address changed",
    "Can not access a needed shared library",
    "Accessing a corrupted shared library",
    ".lib section in a.out corrupted",
    "Attempting to link in too many shared libraries",
    "Cannot exec a shared library directly",
    "Invalid or incomplete multibyte or wide character",
    "Interrupted system call should be restarted",
    "Streams pipe error",
    "Too many users",
    "Socket operation on non-socket",
    "Destination address required",
    "Message too long",
    "Protocol wrong type for socket",
    "Protocol not available",
    "Protocol not supported",
    "Socket type not supported",
    "Operation not supported",
    "Protocol family not supported",
    "Address family not supported by protocol",
    "Address already in use",
    "Cannot assign requested address",
    "Network is down",
    "Network is unreachable",
    "Network dropped connection on reset",
    "Software caused connection abort",
    "Connection reset by peer",
    "No buffer space available",
    "Transport endpoint is already connected",
    "Transport endpoint is not connected",
    "Cannot send after transport endpoint shutdown",
    "Too many references: cannot splice",
    "Connection timed out",
    "Connection refused",
    "Host is down",
    "No route to host",
    "Operation already in progress",
    "Operation now in progress",
    "Stale file handle",
    "Structure needs cleaning",
    "Not a XENIX named type file",
    "No XENIX semaphores available",
    "Is a named type file",
    "Remote I/O error",
    "Disk quota exceeded",
    "No medium found",
    "Wrong medium type",
    "Operation canceled",
    "Required key not available",
    "Key has expired",
    "Key has been revoked",
    "Key was rejected by service",
    "Owner died",
    "State not recoverable",
    "Operation not possible due to RF-kill",
    "Memory page has hardware error"};

#endif

char* s21_strerror(int errorNumber) {
  static char result[512] = {'\0'};

  if (errorNumber < 0 || errorNumber > MAX_ERRLIST) {
    s21_sprintf(result, "Unknown error %d", errorNumber);
  } else {
    s21_strncpy(result, errorList[errorNumber], sizeof(result) - 1);
    result[sizeof(result) - 1] = '\0';
  }

  return result;
}

/* Возвращает указатель на первре вхождение в строку str1
любого символа из str2 или NULL */
char* s21_strpbrk(const char* str1, const char* str2) {
  s21_size_t i;
  int result = __INT_MAX__;
  for (i = 0; i < s21_strlen(str2); i++) {
    const char* ptr = s21_strchr(str1, str2[i]);
    if (ptr != S21_NULL) {
      int offset = ptr - str1;
      if (offset < result) {
        result = offset;
      }
    }
  }
  return result != __INT_MAX__ ? (char*)str1 + result : S21_NULL;
}

/* Выполняет поиск последнего вхождения символа c
(беззнаковый тип) в строке, на которую указывает аргумент str. */
char* s21_strrchr(const char* str, int c) {
  char* result = 0;
  s21_size_t i = s21_strlen(str);
  unsigned char ch = c;

  do {
    if (str[i] == ch) {
      result = (char*)(str + i);
      break;
    }
  } while (i-- > 0);

  return result;
}

char* s21_strstr(const char* haystack, const char* needle) {
  char ch_needle = *needle++;
  s21_size_t needle_len = s21_strlen(needle);

  if (ch_needle == '\0') return (char*)haystack;

  do {
    char ch_haystack;

    do {
      ch_haystack = *haystack++;
      if (ch_haystack == '\0') return (char*)0;
    } while (ch_haystack != ch_needle);
  } while (s21_strncmp(haystack, needle, needle_len) != 0);

  return (char*)(haystack - 1);
}

char* s21_strtok(char* str, const char* delim) {
  char* spanp;  // для перебора символов delim
  int ch_str, ch_delim;
  char* tok;  // текущий токен
  static char* last;  // позиция последнего найденного токена

  if (str == S21_NULL && (str = last) == S21_NULL) {
    return S21_NULL;
  }

  // пропуск разделителей в начале строки
  /* Цикл пропускает любые начальные символы из строки str, которые являются
   * разделителями, указанными в delim. */
  int flag = 1;
  while (flag) {
    flag = 0;
    ch_str = *str++;
    for (spanp = (char*)delim; (ch_delim = *spanp++) != 0;) {
      if (ch_str == ch_delim) {
        flag = 1;
        break;
      }
    }
  }

  // проверка окончания строки
  if (ch_str == 0) {
    last = S21_NULL;
    return S21_NULL;
  }

  // установка начала токена
  tok = str - 1;

  // поиск конца токена
  while (1) {
    ch_str = *str++;
    spanp = (char*)delim;
    do {
      if ((ch_delim = *spanp++) == ch_str) {
        if (ch_str == 0)
          str = S21_NULL;
        else
          str[-1] = 0;
        last = str;
        return tok;
      }
    } while (ch_delim != 0);
  }
}

void* s21_to_upper(const char* str) {
  char* upper_str = S21_NULL;

  if (str != S21_NULL) {
    s21_size_t len = s21_strlen(str);

    upper_str = calloc(s21_strlen(str) + 1, sizeof(char));

    // Преобразование строки в верхний регистр
    for (s21_size_t i = 0; i < len; i++) {
      if (str[i] >= 'a' && str[i] <= 'z') {
        upper_str[i] = str[i] - 32;
      } else {
        upper_str[i] = str[i];
      }
    }

    // Завершение строки нулевым символом
    upper_str[len] = '\0';
  }

  return upper_str;
}

void* s21_to_lower(const char* str) {
  char* lower_str = S21_NULL;

  if (str != S21_NULL) {
    s21_size_t len = s21_strlen(str);

    lower_str = calloc(s21_strlen(str) + 1, sizeof(char));

    for (s21_size_t i = 0; i < len; i++) {
      if (str[i] >= 'A' && str[i] <= 'Z') {
        lower_str[i] = str[i] + 32;
      } else {
        lower_str[i] = str[i];
      }
    }
  }

  return lower_str;
}

void* s21_insert(const char* src, const char* str, s21_size_t start_index) {
  char* res = S21_NULL;
  s21_size_t src_len = (src == S21_NULL) ? 0 : s21_strlen(src);
  s21_size_t str_len = (str == S21_NULL) ? 0 : s21_strlen(str);
  s21_size_t total_len = src_len + str_len;

  if (start_index <= src_len) {
    res = (char*)calloc(src_len + str_len + 1, sizeof(char));
  }
  if (res) {
    for (s21_size_t i = 0; i < total_len; i++) {
      if (i < start_index) {
        res[i] = src[i];
      } else if (i < str_len + start_index) {
        res[i] = str[i - start_index];
      } else {
        res[i] = src[i - str_len];
      }
    }
  }
  return res;
}

// Функция, которая проверяет, начинается ли строка с определенной подстроки
int start_with(const char* str, const char* substr, size_t start) {
  int res = 0;
  int size = s21_strlen(substr);
  for (int i = 0; i < size; i++) {
    if (str[start] == substr[i]) res = 1;
  }
  return res;
}

// Функция, которая проверяет, заканчивается ли строка определенной подстрокой
int end_with(const char* str, const char* substr, size_t end) {
  int res = 0;
  end--;
  int size = s21_strlen(substr);
  for (int i = 0; i < size; i++) {
    if (str[end] == substr[i]) res = 1;
  }
  return res;
}

// Функция, которая обрезает указанные символы из начала и конца строки
void* s21_trim(const char* src, const char* trim_chars) {
  char* newstr = S21_NULL;

  // Проверка на NULL входной строки
  if (src) {
    // Проверка на NULL или пустую строку символов для обрезки
    if (trim_chars && trim_chars[0]) {
      size_t src_len = s21_strlen(src);

      // Выделение памяти под новую строку
      newstr = (char*)calloc(src_len + 1, sizeof(char));

      // Поиск начала и конца подстроки для обрезки
      size_t start = 0, end = src_len;
      while (start_with(src, trim_chars, start)) {
        start++;
      }
      if (start != end) {
        while (end_with(src, trim_chars, end)) {
          end--;
        }
      } else {
        newstr[0] = '\0';
      }

      // Копирование символов из оригинальной строки в новую строку без
      // обрезанных символов
      for (size_t i = 0; start < end; i++, start++) {
        newstr[i] = src[start];
      }
    } else {
      // Если символы для обрезки не указаны, то обрезаем по умолчанию пробелы,
      // табуляции и переносы строк
      newstr = s21_trim(src, "\t\n ");
    }
  }

  return newstr;
}