#pragma once

// Compare two null-terminated strings, examining at most n characters from each
// string.
//   <0: l.substr(0, n) < r.substr(0, n)
//    0: l.substr(0, n) == r.substr(0, n)
//   >0: l.substr(0, n) > r.substr(0, n)
static int strncmp(const char* l, const char* r, unsigned n) {
  unsigned char lc = 0, rc = 0;
  for (unsigned i = 0; i < n && lc == rc; i++) {
    lc = *l++, rc = *r++;
    if (lc == '\0') return lc - rc;
  }
  return lc - rc;
}
