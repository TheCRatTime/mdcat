/*
 * Copyright (C) 2026 TheCRatTime
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>

static char Peek(const char* text) {
  return (*text == '\0' ? '\0' : *(text+1));
}

static const char* HeaderPrint(const char* text) {
  while(*text == '#') {
    text++;
  }

  /* Skipping space */
  if (Peek(text) == ' ') {
    text++;
  }

  fputs("\033[1m", stdout);

  while(*text != '\n' && *text != '\0') {
    putchar(*text);
    text++;
  }

  fputs("\n\033[0m", stdout);

  return text;
}

static const char* PrintColor(const char* text) {
  int star_count = 0;

  if (Peek(text) == ' ') {
    fputs("\033[1m--\033[0m", stdout);
    return text;
  }
  
  while(*text == '*') {
    star_count++;
    text++;
  }

  switch(star_count) {
    case 1:
      fputs("\033[3m", stdout);
      break;
    case 2:
      fputs("\033[1m", stdout);
      break;
    case 3:
      fputs("\033[1;3m", stdout);
      break;
    default:
      fputs("\033[1m", stdout);
      break;
  }

  while(*text != '*' && *text != '\0') {
    putchar(*text);
    text++;
  }

  while(*text == '*') {
    text++;
  }

  fputs("\033[0m", stdout);

  text--;
  
  return text;
}

static const char* PrintCode(const char* text) {
  int quotes_count = 0;
  while(*text == '`') {
    quotes_count++;
    text++;
  }

  if (quotes_count >= 3) {
    /* Skipping lang name */
    while(*text != '\n' && *text != '\0') {
      text++;
    }

    putchar(*(text++));
    
  redo:
    while(*text != '`' && *text != '\0') {
      putchar(*text);
      text++;
    }

    if (*text == '\0') {
      goto end;
    }

    if (Peek(text) == '\0') {
      goto end;
    }

    if (Peek(text+1) == '\0') {
      goto end;
    }

    if (*text != '`' || Peek(text) != '`' || Peek(text+1) != '`') {
      goto redo;
    }

    text = text+3;
  end:;
  }
  else {
    fputs("\033[7m", stdout);
    while(*text != '`' && *text != '\0') {
      putchar(*text);
      text++;
    }

    fputs("\033[0m", stdout);
    text++;
  }

  text--;

  return text;
}

static const char* PrintBlock(const char* text,
                              const char* const first_ptr) {
  if (text != first_ptr) {
    if (*(text-1) != '\n') {
      return text;
    }
  }

  fputs("\033[1m--\033[0m", stdout);

  return text;
}

static const char* PrintBlock2(const char* text,
                               const char* const first_ptr) {
  if (text != first_ptr) {
    if (*(text-1) != '\n') {
      return text;
    }
  }

  if (Peek(text) == ' ') {
    text++;
  }

  fputs("\033[3m  ", stdout);
  while(*text != '\n' && *text != '\0') {
    putchar(*text);
    text++;
  }

  fputs("\033[0m\n", stdout);

  return text;
}


static const char* PrintLink(const char* text) {
  text++;

  fputs("\\fB", stdout);
  
  while(*text != ']' && *text != '\0') {
    putchar(*(text++));
  }

  if (*text == '\0') {
  error:
    fputs("\033[1m", stdout);
    return text;
  }
  
  if (Peek(text) == '\0' || Peek(text+1) == '\0') {
    goto error;
  }
  
  text += 2;

  fputs(" \033[0m(\033[1m", stdout);
  
  while(*text != ')' && *text != '\0') {
    putchar(*(text++));
  }

  fputs("\033[0m)", stdout);
  
  return text;
}

static void PrettyPrint(const char* text) {
  const char* const first_text = text;
  
  while(*text) {
    switch(*text) {
      case '#':
        text = HeaderPrint(text);
        break;
      case '*':
        text = PrintColor(text);
        break;
      case '`':
        text = PrintCode(text);
        break;
      case '-':
        text = PrintBlock(text, first_text);
        break;
      case '>':
        text = PrintBlock2(text, first_text);
        break;
      case '[':
        text = PrintLink(text);
        break;
      default:
        putchar(*text);
        break;
    }
    
    if (*text != '\0') {
      text++;
    }
  }
}

int main(int argc, char** argv) {
  FILE* file = NULL;
  char* buffer = NULL;
  size_t size = 0;
  
  if (argc < 2) {
    printf("Usage: %s FILE\n", argv[0]);
    return 1;
  }

  file = fopen(argv[1], "rb");
  if (file == NULL) {
    perror(argv[0]);
    return 1;
  }

  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer = malloc(size+1);

  if (buffer == NULL) {
    perror(argv[0]);
    fclose(file);
    return 1;
  }

  if (fread(buffer, 1, size, file) <= 0) {
    perror(argv[0]);
    free(buffer);
    fclose(file);
    return 1;
  }

  buffer[size] = '\0';
  fclose(file);

  PrettyPrint(buffer);

  free(buffer);
  return 0;
}
