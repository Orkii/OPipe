// OPipe.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>

#include <chrono>
#include <thread>



//186 ║
//187 ╗
//188 ╝
//200 ╚
//201 ╔
//205 ═


//char symbols[] = "║╗╝╚╔═";

namespace ConsoleForeground {
    enum {
/*0 */        BLACK       = 0,
/*1 */        DARKBLUE    = FOREGROUND_BLUE,
/*2 */        DARKGREEN   = FOREGROUND_GREEN,
/*3 */        DARKCYAN    = FOREGROUND_GREEN | FOREGROUND_BLUE,
/*4 */        DARKRED     = FOREGROUND_RED,
/*5 */        DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
/*6 */        DARKYELLOW  = FOREGROUND_RED | FOREGROUND_GREEN,
/*7 */        DARKGRAY    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
/*8 */        GRAY        = FOREGROUND_INTENSITY,
/*9 */        BLUE        = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
/*10*/        GREEN       = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
/*11*/        CYAN        = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
/*12*/        RED         = FOREGROUND_INTENSITY | FOREGROUND_RED,
/*13*/        MAGENTA     = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
/*14*/        YELLOW      = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
/*15*/        WHITE       = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    };
}

WORD colors[] = {
    ConsoleForeground::BLACK       ,
    ConsoleForeground::DARKBLUE    ,
    ConsoleForeground::DARKGREEN   ,
    ConsoleForeground::DARKCYAN    ,
    ConsoleForeground::DARKRED     ,
    ConsoleForeground::DARKMAGENTA ,
    ConsoleForeground::DARKYELLOW  ,
    ConsoleForeground::DARKGRAY    ,
    ConsoleForeground::GRAY        ,
    ConsoleForeground::BLUE        ,
    ConsoleForeground::GREEN       ,
    ConsoleForeground::CYAN        ,
    ConsoleForeground::RED         ,
    ConsoleForeground::MAGENTA     ,
    ConsoleForeground::YELLOW      ,
    ConsoleForeground::WHITE };

// "║╗╝╚╔═";
const int pipeSet0[] = { 186, 187, 188, 200, 201, 205 };

// "│┐┘└┌─"
const int pipeSet1[] = { 179, 191, 217, 192, 218, 196 };

// "█"
const int pipeSet2[] = { 219, 219, 219, 219, 219, 219 };



HANDLE con; 
COORD minCoord = { 0, 0 };
COORD maxCoord = { 50, 50 };
int siedPercent = 2;
int idle = 10;
#define PIPE_COUNT 1


int getSizeX() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    return columns;
}
int getSizeY() {

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return rows;
}

class Pipe;

class PipeBehavior {
public:
    PipeBehavior() {}
    virtual PipeBehavior* iter(Pipe* pipe) = 0;
};
class PipeBehaviorUp : public PipeBehavior {
public:
    PipeBehavior* iter(Pipe* pipe) override;
};

class PipeBehaviorLeft : public PipeBehavior {
public:
    PipeBehavior* iter(Pipe* pipe) override;
};

class PipeBehaviorRight : public PipeBehavior {
public:
    PipeBehavior* iter(Pipe* pipe) override;
};

class PipeBehaviorDown : public PipeBehavior {
public:
    PipeBehavior* iter(Pipe* pipe) override;
};

class Pipe {
public:
    COORD coord;
    PipeBehavior* pipeBehavior;
    int* pipes;
    WORD color;
    Pipe() {

        pipes = new int[6];

        int rrr = rand() % (3);

        if (rrr == 0) {
            for (int i = 0; i < 6; i++) {
                pipes[i] = pipeSet0[i];
            }
        }
        else if(rrr == 1) {
            for (int i = 0; i < 6; i++) {
                pipes[i] = pipeSet1[i];
            }
        }
        else if (rrr == 2) {
            for (int i = 0; i < 6; i++) {
                pipes[i] = pipeSet2[i];
            }
        }

        color = rand() % (sizeof(colors) / sizeof(WORD));

        int r = rand() % 100;
        if (r > 50) {
            
            if (rand() % 2 == 0) {
                coord.X = 0;
                pipeBehavior = new PipeBehaviorRight();
            }
            else {
                coord.X = maxCoord.X - 1;
                pipeBehavior = new PipeBehaviorLeft();
            }

            


            coord.Y = (rand() % (maxCoord.Y - 2)) + 1;
        }
        else {
            if (rand() % 2 == 0) {
                coord.Y = 0;
                pipeBehavior = new PipeBehaviorDown();
            }
            else {
                coord.Y = maxCoord.Y - 1;
                pipeBehavior = new PipeBehaviorUp();
            }
            coord.X = (rand() % (maxCoord.X-2)) + 1;
        }
    }

    bool iter() {
        SetConsoleTextAttribute(con, color);
        PipeBehavior* a = pipeBehavior->iter(this);
        delete(pipeBehavior);
        pipeBehavior = a;
        
        if (coord.X > maxCoord.X) {
            return false;
        }
        if (coord.Y > maxCoord.Y) {
            return false;
        }
        if (coord.X < minCoord.X) {
            return false;
        }
        if (coord.Y < minCoord.Y) {
            return false;
        }

        return true;
    }

};


int main(){

    //for (int i = 0; i < 255; i++) {
    //    printf("%i %c\n", i, i);
    //}
    //return 1;

    con = GetStdHandle(STD_OUTPUT_HANDLE);


    maxCoord.X = getSizeX();
    maxCoord.Y = getSizeY()-1;

    srand(time(NULL));

    Pipe** pipes = new Pipe*[PIPE_COUNT];

    for (int i = 0; i < PIPE_COUNT; i++) {
        pipes[i] = new Pipe();
    }

    while (true) {
        for (int i = 0; i < PIPE_COUNT; i++) {
            if (pipes[i]->iter() == false) {
                delete(pipes[i]);
                pipes[i] = new Pipe();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(idle));
    }
}

PipeBehavior* PipeBehaviorUp::iter(Pipe* pipe) {
    int r = rand() % 100;
    SetConsoleCursorPosition(con, pipe->coord);
    if (r > 100 - siedPercent) {
        printf("%c", pipe->pipes[4]);  // ╔
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.X++;

        return new PipeBehaviorRight();
    }
    else if (r > 100 - siedPercent - siedPercent) {
        printf("%c", pipe->pipes[1]); // ╗
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.X--;
        return new PipeBehaviorLeft();
    }
    else {
        printf("%c", pipe->pipes[0]); // ║
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.Y--;
        return new PipeBehaviorUp();
    }
}
PipeBehavior* PipeBehaviorLeft::iter(Pipe* pipe){
    int r = rand() % 100;
    SetConsoleCursorPosition(con, pipe->coord);
    if (r > 100 - siedPercent) {
        printf("%c", pipe->pipes[3]);  // ╚
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.Y--;
        return new PipeBehaviorUp();
    }
    else if (r > 100 - siedPercent - siedPercent) {
        printf("%c", pipe->pipes[4]);  // ╔
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.Y++;
        return new PipeBehaviorDown();
    }
    else {
        printf("%c", pipe->pipes[5]);  // ═
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.X--;
        return new PipeBehaviorLeft();
    }
}
PipeBehavior* PipeBehaviorRight::iter(Pipe* pipe){
    int r = rand() % 100;
    SetConsoleCursorPosition(con, pipe->coord);
    if (r > 100 - siedPercent) {
        printf("%c", pipe->pipes[1]); // ╗
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.Y++;
        return new PipeBehaviorDown();
    }
    else if (r > 100 - siedPercent - siedPercent) {
        printf("%c", pipe->pipes[2]);  // ╝  
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.Y--;
        return new PipeBehaviorUp();
    }
    else {
        printf("%c", pipe->pipes[5]);  // ═
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.X++;
        return new PipeBehaviorRight();
    }
}
PipeBehavior* PipeBehaviorDown::iter(Pipe* pipe){
    int r = rand() % 100;
    SetConsoleCursorPosition(con, pipe->coord);
    if (r > 100 - siedPercent) {
        printf("%c", pipe->pipes[2]);  // ╝
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.X--;
        return new PipeBehaviorLeft();
    }
    else if (r > 100 - siedPercent - siedPercent) {
        printf("%c", pipe->pipes[3]);  // ╚
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.X++;
        return new PipeBehaviorRight();
    }
    else {
        printf("%c", pipe->pipes[0]); // ║
        SetConsoleCursorPosition(con, minCoord);
        pipe->coord.Y++;
        return new PipeBehaviorDown();
    }
}