#ifndef LASTLEAVES_MGBA_LOG_H
#define LASTLEAVES_MGBA_LOG_H

#ifdef DEBUG
void mgbalog(const char *format, ...);
#else
inline void mgbalog(const char *format, ...) {}
#endif
#endif //LASTLEAVES_MGBA_LOG_H
