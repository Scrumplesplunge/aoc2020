#pragma once

static int strcmp(const char* l, const char* r) {
  unsigned char lc, rc;
  do {
    lc = *l++, rc = *r++;
    if (lc == '\0') return lc - rc;
  } while (lc == rc);
  return lc - rc;
}
