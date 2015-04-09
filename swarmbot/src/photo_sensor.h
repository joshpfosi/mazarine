enum Colors { RED, BLUE, YELLOW, BLACK };

void test(void);

inline void readRightSensor(int *r, int *b);
inline void readLeftSensor(int  *r, int *b);

inline bool isBlue  (int red, int blue);
inline bool isRed   (int red, int blue);
inline bool isYellow(int red, int blue);

inline Colors getColor(int red, int blue);

inline bool followColorUntilColor(Colors c1, Colors c2);

void setupPhotosensor(void);
