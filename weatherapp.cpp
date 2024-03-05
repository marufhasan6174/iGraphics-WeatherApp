#include "iGraphics.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




char days[8][5] = {"Fri", "Sat", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri"}; // for showing seven days forecast
int day = 0; // day = Friday, this variable for current date which is generate by knowCurrentDay() sync with pc
char date[20];
//Find current day of week
void knowCurrentDay()
{
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *s = asctime(timeinfo);

    char temp[4];
    for(int i = 0; i < 3; i++) 
    {
        temp[i] = s[i];
    }
    temp[4] = '\0';
    for(int i = 1; i < 7; i++)
    {
        if(strcmp(days[i], temp) == 0)
        {
            day = i;
            break;
        }
    }
    strcpy(date, s);

}

int mouseClick = 0, keypress = 0; // for mouse click, keypress,initially both are stop
int selectedCityIndex = -1; // No city selected initially, this will be initialised after cliciking on a particular town(map or by searching)
int showMap = 0; // initially 0, otherwise always 1, 1 means no "select city" string anymore, which refers the first page when starting
char searchInput[50]; // Variable to store user input for search
int wrongInput = 0; //user input validation, whether user input a wrong city name
int blankInput = 0;// if user enter or click on  without any input
int isFound = 0; // show green message if found user typed city
char currentCity[30];
int selectedDay = 1; // first it will show today data
int TempPreciWind = 1;

// define struct for today current data
typedef struct {
    const char* cityName;
    int temperature;
    int humidity;
    int isClear;
    int wind;
} CityWeather;

int n = 9; // no of cities, should be lexicographically as searching loop starts from beggining
CityWeather cities[] = {
    {"Barishal", 25, 65, 1, 10},
    {"Chattogram", 30, 70, 0, 8},
    {"Cumilla", 40, 40, 0, 7},
    {"Dhaka", 22, 60, 1, 9},
    {"Khulna", 28, 75, 0, 5},
    {"Mymensingh", 24, 63, 1, 10},
    {"Rajshahi", 26, 68, 1, 7},
    {"Rangpur", 29, 72, 0, 6},
    {"Sylhet", 27, 78, 0, 9}
};


// for temperature graph temp[time][tmep]
int data1[11];
void tempGraphDataStore(int selectedDay)
{
     FILE *filePointer;

    // Open the file in read mode
    if(selectedDay == 1)
        filePointer = fopen("tempGraphData\\data1.txt", "r");
    if(selectedDay == 2)
        filePointer = fopen("tempGraphData\\data2.txt", "r");
    if(selectedDay == 3)
        filePointer = fopen("tempGraphData\\data3.txt", "r");
    if(selectedDay == 4)
        filePointer = fopen("tempGraphData\\data4.txt", "r");
    if(selectedDay == 5)
        filePointer = fopen("tempGraphData\\data5.txt", "r");
    if(selectedDay == 6)
        filePointer = fopen("tempGraphData\\data6.txt", "r");
    if(selectedDay == 7)
        filePointer = fopen("tempGraphData\\data7.txt", "r");

    if (filePointer == NULL) {
        printf("Error opening the file.\n");
        exit(1); // Exit the program if file opening fails
    }

    int count = 0;
    // Read numbers from the file and store them in the array
    while (fscanf(filePointer, "%d", &data1[count]) != EOF && count < 11) {
        count++;
    }

    fclose(filePointer); // Close the file
}

// temperatures graph showing
void showTempgraph(int day)
{
    tempGraphDataStore(selectedDay);
    iSetColor(255, 233, 222);
    iLine(20, 310+data1[0]*3, 115, 310+data1[1]*3);
    iLine(115, 310+data1[1]*3, 205, 310+data1[2]*3);
    iLine(205, 310+data1[2]*3, 295, 310+data1[3]*3);
    iLine(295, 310+data1[3]*3, 385, 310+data1[4]*3);
    iLine(385, 310+data1[4]*3, 475, 310+data1[5]*3);
    iLine(475, 310+data1[5]*3, 570, 310+data1[6]*3);
    iLine(570, 310+data1[6]*3, 665, 310+data1[7]*3);
    iSetColor(255, 225, 104);
    iText(22, 420, "Temperature |", GLUT_BITMAP_HELVETICA_10);
    iLine(22, 415, 79, 415);
}

void showPrecigraph()
{
    iSetColor(138, 180, 248);
    for(int i = 24, j = 1; i < 700; i += 90, j += 3)
    {
        iText(i, 386, "0%", GLUT_BITMAP_HELVETICA_10);
    }
    iLine(15, 340, 688, 340);

    iSetColor(255, 225, 104);
    iText(84, 420, " Precipitation | ", GLUT_BITMAP_HELVETICA_10);
    iLine(85, 415, 147, 415);
}

void showWindgraph(int day)
{
    char arr[8][50] = {"", "pictures\\wind\\1.bmp", "pictures\\wind\\2.bmp", "pictures\\wind\\3.bmp", "pictures\\wind\\4.bmp", "pictures\\wind\\5.bmp", "pictures\\wind\\6.bmp", "pictures\\wind\\7.bmp"};
    iShowBMP2(7, 328, arr[day], 0);
    iSetColor(255, 255, 255);
    int windValue = (day+25)/2 + selectedCityIndex;
    int flag = 0;
    for(int i = 8; i < 700; i += 90)
    {
        iText(i, 369, itoa(windValue, new char[10], 10));
        if(flag == 0)
        {
            windValue += 2;
            flag = 1;
        }
        else
        {
            flag = 0;
            windValue -= 3;
        }
        iText(i+18, 369, "km/h");
    }
    iSetColor(255, 225, 104);
    iText(155, 420, "Wind", GLUT_BITMAP_HELVETICA_10);
    iLine(155, 415, 179, 415);
}











void TempWindHumidiyFileOpen(int selectedday, int* temp, int* wind, int* humidity)
{
    FILE *filePointer;

    // open temperature data file
    if(selectedday == 1)
        filePointer = fopen("tempGraphData\\data1.txt", "r");
    if(selectedday == 2)
        filePointer = fopen("tempGraphData\\data2.txt", "r");
    if(selectedday == 3)
        filePointer = fopen("tempGraphData\\data3.txt", "r");
    if(selectedday == 4)
        filePointer = fopen("tempGraphData\\data4.txt", "r");
    if(selectedday == 5)
        filePointer = fopen("tempGraphData\\data5.txt", "r");
    if(selectedday == 6)
        filePointer = fopen("tempGraphData\\data6.txt", "r");
    if(selectedday == 7)
        filePointer = fopen("tempGraphData\\data7.txt", "r");

    fscanf(filePointer, "%d", temp);
    fclose(filePointer);



    //open wind data file
    filePointer = fopen("wind\\wind.txt", "r");
    for(int i = 0; i < selectedday; i++)
        fscanf(filePointer, "%d", wind);
    fclose(filePointer);


    //open humidity data file
    filePointer = fopen("humidity\\humidity.txt", "r");
    for(int i = 0; i < selectedday; i++)
        fscanf(filePointer, "%d", humidity);
    fclose(filePointer);

}

void selectedDayIndicator(int day)
{
    // make a rectangle box for the current day which will be the first day (starting day)
    iSetColor(255, 225, 104);
    int x = 0;
    int i;
    for(i = 19; i < 660; i += 94)
    {
        x++;
        if(x == day) break;
    }
    iRectangle(i, 174, 90, 90);
    iSetColor(255, 255, 255);


    // temperature, wind, humidity data showing for day = 0 and forcast for next 6 days 
    int temp, wind, humidity;
    TempWindHumidiyFileOpen(selectedDay, &temp, &wind, &humidity);


    iText(75, 446, itoa(temp+selectedCityIndex%4, new char[10], 10), GLUT_BITMAP_TIMES_ROMAN_24);
    iText(74, 456, "    o");
    iText(116, 452, "C", GLUT_BITMAP_9_BY_15);


    iText(255, 420, itoa(wind + selectedCityIndex%4, new char[10], 10), GLUT_BITMAP_8_BY_13);
    iText(210, 420, "Wind:   km/h", GLUT_BITMAP_8_BY_13);

    iText(210, 440, "Humidity:   %", GLUT_BITMAP_8_BY_13);
    iText(288, 440, itoa(humidity + selectedCityIndex%4, new char[10], 10), GLUT_BITMAP_8_BY_13);
}

void timeInRow(int day)
{
    iSetColor(255, 255, 255);
    for(int i = 17, j = 12; i <= 287; i += 90, j += 3)
    {
        if(j > 12)
        {
            j = 3;
        }
        iText(i, 306, itoa(j, new char[10], 10));
        iText(i, 306, "  AM");
    }
    for(int i = 377, j = 12; i < 700; i += 90, j += 3)
    {
        if(j > 12) j = 3;
        iText(i, 306, itoa(j, new char[10], 10));
        iText(i, 306, "  PM");
    }
}

void isClear(int day)
{
    if(day == 1 || day == 7)
    {
        iText(645, 430, "Clear", GLUT_BITMAP_HELVETICA_12);
        iShowBMP2(22, 430, "pictures\\clear.bmp", 0);
    }
    else if(day == 4 || day == 5)
    {
        iText(612, 430, "Partly Cloud", GLUT_BITMAP_HELVETICA_12);
        iShowBMP2(22, 430, "pictures\\partlycloud.bmp", 0);
    }
    else
    {
        iText(534, 430, "Clear with periodic clouds", GLUT_BITMAP_HELVETICA_12);
        iShowBMP2(22, 430, "pictures\\clear.bmp", 0);
    }
}

void printDay(int selectedDay)
{
    int temp1 = day;
    for(int i = 51, j = 0; i < 690; i += 94, j++)
    {
        if((temp1 + j) > 7)
        {
            temp1 = 0;
            j = 1;
        }
        iText(i, 250, days[temp1+j], GLUT_BITMAP_8_BY_13);
    }
    iText(610, 470, "Weather", GLUT_BITMAP_HELVETICA_18);
    char temp[10];
    for(int i = 0; i < 10; i++)
        temp[i] = date[i];
    iText(85, 560, temp);
    iText(25, 560, "TODAY : ");
    iText(165, 560, ", 2024");
    iText(109, 560, ",");

    int x = (day+selectedDay-1-1)%7;
    switch (x)
    {
        case -1:
            iText(638, 450, "Friday", GLUT_BITMAP_HELVETICA_12);
            break;
        case 0:
            iText(625, 450, "Saturday", GLUT_BITMAP_HELVETICA_12);
            break;
        case 1:
            iText(635, 450, "Sunday", GLUT_BITMAP_HELVETICA_12);
            break;
        case 2:
            iText(635, 450, "Monday", GLUT_BITMAP_HELVETICA_12);
            break;
        case 3:
            iText(630, 450, "Tuesday", GLUT_BITMAP_HELVETICA_12);
            break;
        case 4:
            iText(614, 450, "Wednesday", GLUT_BITMAP_HELVETICA_12);
            break;
        case 5:
            iText(628, 450, "Thursday", GLUT_BITMAP_HELVETICA_12);
            break;
        case 6:
            iText(638, 450, "Friday", GLUT_BITMAP_HELVETICA_12);
            break;
    }

}


// function to read feels like temperature
int feelsLike(int newselectedday)
{
    int feels, x, y;
    TempWindHumidiyFileOpen(newselectedday, &feels, &x, &y);
    return feels;
}


void drawWeatherInfo(CityWeather cityData) {

    iShowBMP(0, 0, "pictures\\weatherbackground.bmp");
    iText(22, 420, "Temperature |", GLUT_BITMAP_HELVETICA_10);
    iText(84, 420, " Precipitation | ", GLUT_BITMAP_HELVETICA_10);
    iText(155, 420, "Wind", GLUT_BITMAP_HELVETICA_10);
    if(TempPreciWind == 1)
    {
        showTempgraph(selectedDay);
    }
    else if(TempPreciWind == 2)
    {
        showPrecigraph();
    }
    else
    {
        showWindgraph(selectedDay);
    }
    
    selectedDayIndicator(selectedDay);

    timeInRow(selectedDay);

    isClear(selectedDay);

    printDay(selectedDay);

    // show weather data
    iText(210, 460, "Precipitation: 0%", GLUT_BITMAP_8_BY_13);

    iSetColor(255, 255, 255);

	// Add a button to go back to the main menu
    iRectangle(270, 120, 135, 30);
    iText(280, 128, "Back to Map", GLUT_BITMAP_HELVETICA_18);

    iText(270, 530, "City Name: ", GLUT_BITMAP_HELVETICA_18);
    iText(370, 530, (char*)cityData.cityName, GLUT_BITMAP_HELVETICA_18);

    for(int i = 39; i < 690; i += 94)
    {
        if(i == 321 || i == 415) continue;
        iShowBMP2(i, 200, "pictures\\clear.bmp", 0);
    }
    for(int i = 321; i <= 415; i += 94)
        iShowBMP2(i, 200, "pictures\\partlycloud.bmp", 0);
    iSetColor(0, 255, 0);
    iFilledCircle(64, 225, 20, 10000);
    iSetColor(255, 255, 255);


    // feels like temperature
    int tempday = 1;
    for(int i = 35; i < 660; i += 94)
    {
        tempday %= 8;
        if(tempday == 0) tempday = 1;
        iText(i, 180, itoa(feelsLike(tempday), new char[10], 10));
        iText(i+17, 184, "o");
        iText(i+37, 180, itoa(feelsLike(tempday)-5, new char[10], 10));
        iText(i+54, 184, "o");
        tempday++;
    }
    iSetColor(255, 255, 255);
}



void iDraw() {
    iClear();
    // play sound for mouse click and keypress
    if(mouseClick == 1)
    {
        PlaySound(TEXT("sounds\\mouse-click.wav"), NULL, SND_FILENAME | SND_ASYNC);
        mouseClick = 0;
    }
    if(keypress == 1)
    {
        PlaySound(TEXT("sounds\\keyboard.wav"), NULL, SND_FILENAME | SND_ASYNC);
        keypress = 0;
    }
    iSetColor(055, 113, 128);
    iFilledRectangle(0, 0, 700, 600);

    // Draw city selection drop-down menu
    iSetColor(255, 255, 255);
        iShowBMP(0, 0, "pictures\\bc2.bmp");
	if(showMap == 0)
	{
        iShowBMP(0, 0, "pictures\\bc.bmp");
		iText(165, 150, "For Map View:", GLUT_BITMAP_HELVETICA_18);
		iRectangle(300, 141, 120, 30);
        iText(320, 151, "click here", GLUT_BITMAP_HELVETICA_18);
        iText(280, 105, "OR", GLUT_BITMAP_HELVETICA_18);
	}
    if (selectedCityIndex != -1) {
        drawWeatherInfo(cities[selectedCityIndex]);
    }

    // Draw city options in the drop-down menu
    if (selectedCityIndex == -1) {
		iSetColor(255, 255, 255);
		if(showMap == 1)
		{
            iShowBMP2(100, 0, "pictures\\map.bmp", 0);
			iText(240, 568, "\" Select \" or \" Search by typing \"", GLUT_BITMAP_HELVETICA_18);
		}
    }

    // Draw search input box
    iSetColor(255, 255, 255);
    iRectangle(150, 50, 230, 30);
    iText(160, 60, "Search City:", GLUT_BITMAP_HELVETICA_18);
    iSetColor(0, 255, 0);
    iText(270, 60, searchInput, GLUT_BITMAP_HELVETICA_18);

    // Draw search button
    iSetColor(255, 255, 255);
    iRectangle(400, 50, 80, 30);
    iText(410, 60, "Search", GLUT_BITMAP_HELVETICA_18);

    // handling wrong input from user
    if(wrongInput)
    {
        iSetColor(255, 0, 0);
        iText(270, 60, "Wrong Input", GLUT_BITMAP_HELVETICA_18);
    }
    if(blankInput)
    {
        iSetColor(255, 0, 0);
        iText(270, 60, "blank input", GLUT_BITMAP_HELVETICA_18);
    }
    if(isFound)
    {
        iSetColor(0, 255, 0);
        iText(270, 60, "found", GLUT_BITMAP_HELVETICA_18);
    }
}


void iMouseMove(int mx, int my) {
}


void iMouse(int button, int state, int mx, int my) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Check if the mouse click is within the Show map -> click here
        if (mx > 299 && mx < 421 && my > 139 && my < 171 && showMap == 0) {
            showMap = 1;
            wrongInput = 0;
            blankInput = 0;
            isFound = 0;
            mouseClick = 1;
        }
        // Hide detailed weather info and go back to the map -> Back to Map
		else if (selectedCityIndex != -1 && mx > 270 && mx < 405 && my > 120 && my < 150) {
            selectedCityIndex = -1; 
            wrongInput = 0;
            blankInput = 0;
            isFound = 0;
            mouseClick = 1;
            selectedDay = 1;
            TempPreciWind = 1;
        }
        // Check if the mouse click is on the search button -> Search
        else if (mx > 400 && mx < 480 && my > 50 && my < 80) {
            mouseClick = 1;
            int i;
            for(i = 0; i < n; i++)
            {
                int len = strlen(searchInput);
                int j;
                int flag = 0;
                for(j = 0; j < len; j++)
                {
                    if(tolower(searchInput[j]) != tolower(cities[i].cityName[j]))
                        break;
                    flag = 1;
                }
                if(j == len && flag)
                {
                    selectedCityIndex = i;
                    isFound = 1;
                    showMap = 1;
                    selectedDay = 1;
                    TempPreciWind = 1;
                    break;
                }
            }

            // handling wrong and blank input
            if(i == n)
            {
                if(strlen(searchInput) == 0)
                {
                    wrongInput = 0;
                    blankInput = 1;
                }
                else
                {
                    wrongInput = 1;
                    blankInput = 0;
                }
                isFound = 0;
            }
            memset(searchInput, 0, sizeof(searchInput));
        }
        // Check if the mouse click is on any of the city options in map -> Click on map
        else if (selectedCityIndex == -1 && showMap == 1) {
            if(mx > 305 && mx < 385 && my > 140 && my < 200)
            {
                selectedCityIndex = 0;
                mouseClick = 1;
            }
            else if(mx > 475 && mx < 545 && my > 142 && my < 252)
            {
                selectedCityIndex = 1;
                mouseClick = 1;
            }
            else if(mx > 375 && mx < 435 && my > 232 && my < 292)
            {
                selectedCityIndex = 2;
                mouseClick = 1;
            }
            else if(mx > 305 && mx < 360 && my > 282 && my < 342)
            {
                selectedCityIndex = 3;
                mouseClick = 1;
            }
            else if(mx > 205 && mx < 285 && my > 160 && my < 260)
            {
                selectedCityIndex = 4;
                mouseClick = 1;
            }
            else if(mx > 315 && mx < 375 && my > 382 && my < 432)
            {
                selectedCityIndex = 5;
                mouseClick = 1;
            }
            else if(mx > 175 && mx < 255 && my > 352 && my < 412)
            {
                selectedCityIndex = 6;
                mouseClick  = 1;
            }
            else if(mx > 175 && mx < 275 && my > 462 && my < 522)
            {
                selectedCityIndex = 7;
                mouseClick = 1;
            }
            else if(mx > 425 && mx < 525 && my > 362 && my < 412)
            {
                selectedCityIndex = 8;
                mouseClick = 1;
            }
        }
    
        // working on weather main interface
        else if(selectedCityIndex != -1)
        {
            // select different days in next 7 days
            int day = 1;
            for(int i = 19; i < 660; i += 94)
            {
                if(mx > i && mx < (i+90) && my > 174 && my < (174+90))
                {
                    selectedDay = day;
                    mouseClick = 1;
                }
                day++;
            }


            // temperature, precipitation and wind section
            {
                if(mx > 22 && mx < 79 && my > 410 && my < 435)
                {
                    TempPreciWind = 1;
                    mouseClick = 1;
                }
                if(mx > 85 && mx < 147 && my > 410 && my < 435)
                {
                    TempPreciWind = 2;
                    mouseClick = 1;
                }
                if(mx > 155 && mx < 179 && my > 410 && my < 435)
                {
                    TempPreciWind = 3;
                    mouseClick = 1;
                }
            }
        }
    }
}

void iKeyboard(unsigned char key) {
    if (key == '\r') { // Enter key pressed
        keypress = 1;
        if(strcmp("exit", searchInput) == 0) exit(0); // close the app by typing "exit"
        int i;
        for(i = 0; i < n; i++)
        {
            int len = strlen(searchInput);
            int j;
            int inLoop = 0;
            for(j = 0; j < len; j++)
            {
                inLoop = 1;
                if(tolower(searchInput[j]) != tolower(cities[i].cityName[j]))
                    break;
            }
            if(j == len && inLoop != 0)
            {
                selectedCityIndex = i;
                isFound = 1;
                showMap = 1;
                selectedDay = 1;
                TempPreciWind = 1;

                break;
            }
        }
        //if user type a wrong input 
        if(i == n)
        {
            if(strlen(searchInput) != 0)
            {
                wrongInput = 1;
                blankInput = 0;
            }
            else
            {
                wrongInput = 0;
                blankInput = 1;
            }
            isFound = 0;
        }
        memset(searchInput, 0, sizeof(searchInput));

    } else if (key == 8) { // Backspace key pressed
        keypress = 1;    
        int len = strlen(searchInput);
        if (len > 0) {
            searchInput[len - 1] = '\0'; // removing last character replacing by null
        }
    } else {
        keypress = 1;
        strncat(searchInput, (char*)&key, 1);
        wrongInput = 0;
        blankInput = 0;
        isFound = 0;
    }
}

void iSpecialKeyboard(unsigned char key) {
}


int main() {
    PlaySound(TEXT("sounds\\start.wav"), NULL, SND_SYNC | SND_ASYNC); // starting sound of the app
    knowCurrentDay();
    iInitialize(700, 600, "Weather App");
    return 0;
}