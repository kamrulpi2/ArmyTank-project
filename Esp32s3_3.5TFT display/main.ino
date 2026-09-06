#include <TFT_eSPI.h>
#include <math.h>


TFT_eSPI tft = TFT_eSPI();


//================================================
// TIMING
//================================================

#define BOOT_TIME 15000        // Quantum Spark intro
#define PAGE_TIME 5000         // Slide time
#define ANIM_TIME 120          // Animation speed



//================================================
// COLORS
//================================================

#define BG_COLOR       0x0841
#define HEADER_COLOR   0x19F7
#define CARD_COLOR     0x2104

#define WHITE   TFT_WHITE
#define BLACK   TFT_BLACK
#define GREEN   TFT_GREEN
#define RED     TFT_RED
#define YELLOW  TFT_YELLOW
#define CYAN    TFT_CYAN
#define BLUE    TFT_BLUE
#define ORANGE  TFT_ORANGE



//================================================
// PAGE SYSTEM
//================================================

volatile int currentPage = 0;

#define TOTAL_PAGES 6


unsigned long pageTimer = 0;
unsigned long animTimer = 0;



//================================================
// ANIMATION VARIABLES
//================================================

int waterWave = 0;

int pipeFlow = 0;

int pumpAngle = 0;

int radarAngle = 0;


bool beaconState = false;



//================================================
// SENSOR VALUES
//================================================

float waterLevel = 78.0;

float temperature = 32.0;

float pressure = 1.8;

float flowRate = 25.0;

float pumpCurrent = 4.2;

float voltage = 12.4;



//================================================
// RTOS TASK HANDLES
//================================================

TaskHandle_t UI_Task_Handle;

TaskHandle_t Animation_Task_Handle;

TaskHandle_t Sensor_Task_Handle;



//================================================
// QUANTUM SPARK STARTUP
//================================================

void quantumSparkBoot()
{

unsigned long start = millis();


while(millis() - start < BOOT_TIME)
{

tft.fillScreen(BLACK);


tft.setTextDatum(MC_DATUM);


// logo

tft.setTextColor(CYAN);

tft.drawString(
"QUANTUM SPARK",
240,
110,
6
);



delay(250);



tft.fillScreen(BLACK);



tft.setTextColor(YELLOW);

tft.drawString(
"QUANTUM SPARK",
240,
110,
6
);



delay(250);



// subtitle

tft.setTextColor(WHITE);


tft.drawString(
"RESCUE TANK CONTROL SYSTEM",
240,
170,
2
);



// loading bar

tft.drawRoundRect(
70,
220,
340,
25,
8,
WHITE
);



int progress = map(
millis()-start,
0,
BOOT_TIME,
0,
330
);



tft.fillRoundRect(
75,
225,
progress,
15,
5,
GREEN
);



tft.setTextColor(GREEN);


tft.drawString(
"SYSTEM INITIALIZING",
240,
280,
2
);


}


tft.fillScreen(BG_COLOR);

}



//================================================
// SETUP
//================================================

void setup()
{

Serial.begin(115200);



pinMode(8,OUTPUT);

digitalWrite(8,HIGH);



tft.init();

tft.setRotation(1);



quantumSparkBoot();



// RTOS TASKS


xTaskCreatePinnedToCore(
UI_Task,
"UI_TASK",
8192,
NULL,
2,
&UI_Task_Handle,
1
);



xTaskCreatePinnedToCore(
Animation_Task,
"ANIMATION_TASK",
4096,
NULL,
1,
&Animation_Task_Handle,
1
);



xTaskCreatePinnedToCore(
Sensor_Task,
"SENSOR_TASK",
4096,
NULL,
1,
&Sensor_Task_Handle,
0
);



}



//================================================
// LOOP
//================================================

void loop()
{

vTaskDelay(portMAX_DELAY);

}
//================================================
// UI TASK
//================================================

void UI_Task(void *parameter)
{

while(true)
{


if(millis()-pageTimer >= PAGE_TIME)
{

currentPage++;


if(currentPage >= TOTAL_PAGES)
{
currentPage = 0;
}


drawCurrentPage();


pageTimer = millis();

}


vTaskDelay(
100 / portTICK_PERIOD_MS
);


}

}



//================================================
// PAGE CONTROLLER
//================================================

void drawCurrentPage()
{

tft.fillScreen(BG_COLOR);



switch(currentPage)
{

case 0:
drawDashboard();
break;


case 1:
drawTank();
break;


case 2:
drawPump();
break;


case 3:
drawSensors();
break;


case 4:
drawEmergency();
break;


case 5:
drawInfo();
break;


}

}



//================================================
// HEADER
//================================================

void header(String title)
{

tft.fillRect(
0,
0,
480,
45,
HEADER_COLOR
);



tft.setTextDatum(MC_DATUM);

tft.setTextColor(WHITE);


tft.drawString(
title,
240,
22,
4
);


}



//================================================
// CARD
//================================================

void card(
int x,
int y,
String name,
String value,
uint16_t color
)
{


tft.fillRoundRect(
x,
y,
210,
80,
10,
CARD_COLOR
);



tft.setTextDatum(TL_DATUM);



tft.setTextColor(CYAN);


tft.drawString(
name,
x+15,
y+10,
2
);



tft.setTextColor(color);


tft.drawString(
value,
x+15,
y+42,
4
);



}



//================================================
// PAGE 0
// MAIN DASHBOARD
//================================================

void drawDashboard()
{

header("QUANTUM SPARK");



card(
20,
70,
"SYSTEM",
"ONLINE",
GREEN
);



card(
250,
70,
"MODE",
"AUTO",
CYAN
);



card(
20,
190,
"PUMP",
"RUNNING",
GREEN
);



card(
250,
190,
"VALVE",
"OPEN",
YELLOW
);



// bottom status

tft.fillRoundRect(
40,
285,
400,
25,
8,
GREEN
);



tft.setTextDatum(MC_DATUM);

tft.setTextColor(BLACK);


tft.drawString(
"RESCUE READY",
240,
298,
2
);


}



//================================================
// PAGE 1
// 3D TANK
//================================================

void drawTank()
{

header("3D TANK MONITOR");



// shadow

tft.fillEllipse(
140,
260,
80,
15,
0x4208
);



// tank body

tft.drawRoundRect(
70,
90,
140,
150,
20,
WHITE
);



// tank dome

tft.drawEllipse(
140,
90,
70,
25,
WHITE
);



// water

tft.fillRoundRect(
80,
145,
120,
85,
10,
BLUE
);



tft.drawLine(
90,
165,
190,
165,
CYAN
);



tft.drawLine(
90,
190,
190,
190,
CYAN
);



// data

card(
270,
80,
"LEVEL",
"78%",
YELLOW
);



card(
270,
180,
"TEMP",
"32 C",
ORANGE
);



// outlet pipe

tft.drawLine(
210,
180,
270,
180,
WHITE
);


}



//================================================
// PAGE 2
// FIXED PUMP STATION
//================================================

void drawPump()
{

header("PUMP STATION");



// pump status cards

card(
20,
70,
"PUMP 01",
"ACTIVE",
GREEN
);



card(
250,
70,
"PUMP 02",
"READY",
YELLOW
);



// pipeline

tft.drawLine(
50,
240,
300,
240,
WHITE
);



// valve

tft.drawCircle(
300,
240,
12,
CYAN
);



// pump body

tft.fillCircle(
390,
220,
50,
CARD_COLOR
);



tft.drawCircle(
390,
220,
50,
WHITE
);



// motor center

tft.fillCircle(
390,
220,
12,
GREEN
);



// information

card(
20,
170,
"FLOW",
"25 L/M",
CYAN
);



card(
250,
170,
"POWER",
"12.4V",
ORANGE
);



}



//================================================
// PAGE 3
// SENSOR MONITOR
//================================================

void drawSensors()
{

header("LIVE SENSOR DATA");



card(
20,
70,
"WATER",
"78 %",
BLUE
);



card(
250,
70,
"TEMP",
"32 C",
ORANGE
);



card(
20,
190,
"PRESSURE",
"1.8 BAR",
CYAN
);



card(
250,
190,
"CURRENT",
"4.2 A",
GREEN
);



}



//================================================
// PAGE 4
// EMERGENCY CENTER
//================================================

void drawEmergency()
{

header("EMERGENCY CENTER");



// radar base

tft.drawCircle(
140,
160,
80,
GREEN
);



tft.drawCircle(
140,
160,
45,
GREEN
);



tft.drawCircle(
140,
160,
10,
GREEN
);



tft.setTextColor(CYAN);


tft.drawString(
"RADAR",
110,
260,
2
);



// status panel

tft.fillRoundRect(
280,
70,
170,
170,
10,
CARD_COLOR
);



tft.setTextColor(WHITE);



tft.drawString(
"FIRE",
300,
95,
2
);



tft.drawString(
"GAS",
300,
135,
2
);



tft.drawString(
"LEAK",
300,
175,
2
);



tft.drawString(
"POWER",
300,
215,
2
);



// status lights

tft.fillCircle(
410,
105,
10,
GREEN
);



tft.fillCircle(
410,
145,
10,
GREEN
);



tft.fillCircle(
410,
185,
10,
GREEN
);



tft.fillCircle(
410,
225,
10,
GREEN
);



// alarm bar

tft.fillRoundRect(
40,
285,
400,
25,
8,
GREEN
);



tft.setTextDatum(MC_DATUM);

tft.setTextColor(BLACK);


tft.drawString(
"SYSTEM NORMAL",
240,
298,
2
);



// beacon

tft.fillCircle(
420,
55,
18,
RED
);



}



//================================================
// PAGE 5
// SYSTEM INFORMATION
//================================================

void drawInfo()
{

header("SYSTEM INFO");



tft.setTextColor(WHITE);



tft.drawString(
"NAME : QUANTUM SPARK",
40,
90,
2
);



tft.drawString(
"BOARD : ESP32-S3 N16R8",
40,
130,
2
);



tft.drawString(
"DISPLAY : ILI9488",
40,
170,
2
);



tft.drawString(
"RTOS : ACTIVE",
40,
210,
2
);



tft.setTextColor(GREEN);



tft.drawString(
"ALL SYSTEM READY",
120,
280,
3
);


}
//================================================
// ANIMATION TASK
//================================================

void Animation_Task(void *parameter)
{

while(true)
{


updateAnimation();



vTaskDelay(
ANIM_TIME / portTICK_PERIOD_MS
);


}

}



//================================================
// SENSOR TASK
//================================================

void Sensor_Task(void *parameter)
{

while(true)
{


// Demo sensor simulation

waterLevel += 0.2;


if(waterLevel > 90)
{
waterLevel = 70;
}



temperature += 0.05;


if(temperature > 38)
{
temperature = 30;
}



pressure += 0.01;


if(pressure > 2.5)
{
pressure = 1.5;
}



flowRate += 0.5;


if(flowRate > 35)
{
flowRate = 20;
}



pumpCurrent += 0.05;


if(pumpCurrent > 6)
{
pumpCurrent = 3;
}



vTaskDelay(
2000 / portTICK_PERIOD_MS
);


}

}



//================================================
// ANIMATION ENGINE
//================================================

void updateAnimation()
{


//================================================
// TANK WATER ANIMATION
//================================================

if(currentPage == 1)
{


waterWave++;


if(waterWave > 15)
{
waterWave = 0;
}



// clear only water area

tft.fillRect(
82,
145,
116,
80,
BLUE
);



// draw moving waves

for(int i=0;i<3;i++)
{

tft.drawLine(
90,
160+(i*20)+waterWave,
190,
160+(i*20)+waterWave,
CYAN
);


}


}



//================================================
// PUMP STATION ANIMATION
//================================================

if(currentPage == 2)
{


//----------------------
// Motor rotation
//----------------------


// clear rotor area only

tft.fillCircle(
390,
220,
35,
CARD_COLOR
);



// restore pump ring

tft.drawCircle(
390,
220,
50,
WHITE
);



tft.fillCircle(
390,
220,
12,
GREEN
);



pumpAngle += 15;


if(pumpAngle >= 360)
{
pumpAngle = 0;
}



int motorX =
390 +
cos(pumpAngle * 0.01745)
*
28;



int motorY =
220 +
sin(pumpAngle * 0.01745)
*
28;



tft.drawLine(
390,
220,
motorX,
motorY,
GREEN
);




//----------------------
// Pipeline water flow
//----------------------


// clear flow line

tft.fillRect(
60,
230,
230,
20,
BG_COLOR
);



// redraw pipe

tft.drawLine(
50,
240,
300,
240,
WHITE
);



pipeFlow++;


if(pipeFlow > 40)
{
pipeFlow = 0;
}



for(int i=0;i<5;i++)
{


int flowX =
70 +
(i*40)
+
pipeFlow;



if(flowX > 290)
{
flowX = 70;
}



tft.fillCircle(
flowX,
240,
4,
CYAN
);



}


}



//================================================
// EMERGENCY RADAR + BEACON
//================================================

if(currentPage == 4)
{


// clear radar sweep

tft.fillCircle(
140,
160,
78,
BG_COLOR
);



// radar circles

tft.drawCircle(
140,
160,
80,
GREEN
);


tft.drawCircle(
140,
160,
45,
GREEN
);



radarAngle += 8;


if(radarAngle >= 360)
{
radarAngle = 0;
}



int radarX =
140 +
cos(radarAngle * 0.01745)
*
75;



int radarY =
160 +
sin(radarAngle * 0.01745)
*
75;



tft.drawLine(
140,
160,
radarX,
radarY,
CYAN
);



// beacon flash


beaconState = !beaconState;



tft.fillCircle(
420,
55,
18,
beaconState ? RED : YELLOW
);



tft.drawCircle(
420,
55,
22,
WHITE
);



}


}
