#ifndef DISPLAY_H_
#define DISPLAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

void initSSD1306i2c(void *ignore);
void displayData();

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H_ */