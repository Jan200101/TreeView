#ifndef MAIN_H
#define MAIN_H

extern int done;
extern char root_dir[];

void processSDLEvent();
int init();
int app(int, char**);

#endif
