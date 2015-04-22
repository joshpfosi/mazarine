#ifndef PHOTO_SENSOR_H
#define PHOTO_SENSOR_H

enum Colors { RED, BLUE, YELLOW, BLACK };

Colors readRightSensor (void);
Colors readLeftSensor  (void);

bool isBlue  (int red, int blue);
bool isRed   (int red, int blue);
bool isYellow(int red, int blue);

Colors getColor(int red, int blue);

void actionUntilColor(Colors c, void (*action)(void));
bool followColorUntilColor(Colors c1, Colors c2);

void setupPhotosensor(void);
void testPhotosensor(void);

#endif // PHOTO_SENSOR_H
