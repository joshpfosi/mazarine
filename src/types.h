#define FALSE         0
#define TRUE          1

/* pins */
#define RED_LED       6
#define BLUE_LED      4
#define GREEN_LED     8
#define PIN1          0
#define PIN2          1
#define POTPIN1       A0
#define POTPIN2       A1

/* misc */
#define LED_ON        255
#define LED_OFF       0
#define SLEEP_DUR     15000

/* frequencies */
#define TEN_HERTZ     50
#define FOUR_HERTZ    125
#define TWO_HERTZ     250
#define ONE_HERTZ     500

enum States { ON, OFF, RUN, SLEEP, DIAG };
