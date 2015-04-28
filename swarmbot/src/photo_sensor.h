#ifndef PHOTO_SENSOR_H
#define PHOTO_SENSOR_H

enum Colors { RED, BLUE, YELLOW, BLACK };

void readSensors      (Colors &left, Colors &right);
void setupPhotosensor (void);
void testPhotosensor  (void);

#endif // PHOTO_SENSOR_H
