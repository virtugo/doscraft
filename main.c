#include<stdio.h>
#include<stdlib.h>

int game();
int WorldGen(int xMax, int yMax, int arrWorld[xMax][yMax], int *x, int *y, int *goWhere, int *intBlocks);
int RedrawScreen(int xMax, int yMax, int arrFirstWorld[xMax][yMax], int *x, int *y, int *goWhere, int intMode, int *intBlocks);
char GetKey(int *goWhere);
int SkyGen(int xMax, int yMax, int arrWorld[xMax][yMax]);
int FloorGen(int xMax, int yMax, int arrWorld[xMax][yMax]);
int HeroGen(int xMax, int yMax, int arrWorld[xMax][yMax], int **x, int **y);

// ========================== MAIN ============================
int main()
{
    game();
    return 0;
}
// ========================== <GAME> =======================
int game()
{
    typedef enum {CONTINUE, QUIT, RESTART, DIGMODE, TRANSPARENT, BUILDMODE, WALKMODE} enumTodo;
    enumTodo todo = RESTART;

    int intMode = 0; // режим: копать(1), летать и тд

    int xMax=70; // размер мира по x
    int yMax=16; // размер мира по y (>=10)
    int arrFirstWorld[xMax][yMax]; // новый мир (в процессе игры меняется)
    int x,y; // координаты героя
    int intBlocks; // количество блоков в инвентаре

    int goWhere; // направление движения. 0 стоять на месте

    // запускаем процесс игры

    do {
        // генерируем мир, если надо
        if(todo==RESTART){
            WorldGen(xMax, yMax, arrFirstWorld, &x, &y, &goWhere, &intBlocks);
            todo = CONTINUE;
        }
        // перерисовываем экран
        RedrawScreen(xMax, yMax, arrFirstWorld, &x, &y, &goWhere, intMode, &intBlocks);

        // ждем нажатий
        todo = GetKey(&goWhere);
        // включаем/выключаем режим копания
        if(todo==DIGMODE){
            if(intMode==1)intMode = 0;
            else intMode = 1;
        }
        // включаем/выключаем прозрачный режим
        else if(todo==TRANSPARENT){
            if(intMode==9)intMode = 0;
            else intMode = 9;
        }
        // включаем/выключаем режим постройки
        else if(todo==BUILDMODE){
            if(intMode==2)intMode = 0;
            else intMode = 2;
        }
        // включаем обычный режим
        else if(todo==WALKMODE){
            intMode = 0;
        }
    } while (todo!=QUIT);
}
// ========================== </GAME> =======================

int WorldGen(int xMax, int yMax, int arrWorld[xMax][yMax], int *x, int *y, int *goWhere, int *intBlocks){
    // стоять на месте
    *goWhere = 0;
    // обнуляем блоки
    *intBlocks = 0;

    // генерируем небо
    SkyGen(xMax, yMax, arrWorld);
    // генерируем землю
    FloorGen(xMax, yMax, arrWorld);
    // генерируем человечка
    HeroGen(xMax, yMax, arrWorld, &x, &y);
// возвращаемся в game
}

// ------------------ <REDRAW SCREEN> ----------------------------

int RedrawScreen(int xMax, int yMax, int arrFirstWorld[xMax][yMax], int *x, int *y, int *goWhere, int intMode, int *intBlocks){

    int arrWorld[xMax][yMax]; // мир в данный момент
    // вычисления
        // вычисляем бэкграунд
        calcBackgr(xMax, yMax, arrFirstWorld, arrWorld);
        // вычисляем положение человечка
        calcHeroPos(xMax, yMax, arrFirstWorld, arrWorld, &x, &y, &goWhere, intMode, &intBlocks);
    // отрисовка
        // обнуляем экран
        system("cls");
        // рисуем меню топ
            printf("INVENTORY: %d\n", *intBlocks);
            // если обычный режим
            if(intMode==0)printf("WALK MODE\n");
            // если режим копания
            else if(intMode==1)printf("DIG MODE\n");
            else if(intMode==2)printf("BUILD MODE\n");
            else if(intMode==9)printf("TRANSPARENT MODE\n");
        // рисуем мир
        DrawWorld(xMax, yMax, arrWorld);
        // рисуем меню низ
        printf("BOTTOM MENU\n");
        printf("Press 1 for walk mode\n");
        printf("Press 2 for dig mode\n");
        printf("Press 3 for build mode\n");
        printf("Press 9 for transparent mode\n");
        printf("Press r to restart\n");
        printf("Press q to quit");
// возвращаемся в game
}
// ------------------ </REDRAW SCREEN> ----------------------------
int calcHeroPos(int xMax, int yMax, int arrFirstWorld[xMax][yMax], int arrWorld[xMax][yMax], int **x, int **y, int **goWhere, int intMode, int **intBlocks){

    int y1 = **y; // "y" для дебаггера

    // если нажали не на wasd
    if(**goWhere==0){
        // переключили режим в WALK
        if(intMode==0){
            // если не находимся на нижней границе
            // проверить, нужен yMax или (yMax-1)
            if(y1<(yMax-1)){
                // включаем гравитацию
                // проверяем, есть ли под героем воздух
                // если есть
                if(arrWorld[**x][y1+1]=="."){
                    // ищем под ним землю
                    do {
                        // переходим на клетку с землей
                        arrFirstWorld[**x][y1]=".";
                        arrWorld[**x][y1]=".";
                        y1++;
                    // делаем, пока не дойдем до земли
                    // или до границы
                    } while ((arrWorld[**x][y1+1]!="n")&&(arrWorld[**x][y1+1]!="m")&&(y1+1<yMax));
                    **y = y1;
                }
            }
        }
    }
    // если нажали на wasd
    // ------------ ВВЕРХ -----------
    else if(**goWhere==1){
            // если режим копания
            if (intMode==1){
                // если не дошёл до верхней границы карты
                if(**y>0){
                    // если наверху земля
                    if((arrWorld[**x][**y-1]=="n")||(arrWorld[**x][**y-1]=="m")){
                        //теперь в клетке воздух
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**y)--;
                        // добавляем землю в инвентарь
                        (**intBlocks)++;
                    }
                    // если наверху воздух
                    else if (arrWorld[**x][**y-1]=="."){
                        //теперь в текущей клетке воздух
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**y)--;
                    }
                }
            }
            // если режим постройки
            else if (intMode==2){
                // если не дошёл до верхней границы карты
                if(**y>0){
                    // если воздух
                    if(arrWorld[**x][**y-1]=="."){
                        // если инвентарь не пуст
                        if((**intBlocks)>0){
                            //теперь в старой клетке земля
                            arrWorld[**x][**y]="n";
                            arrFirstWorld[**x][**y]="n";
                            // перемещаем наверх
                            (**y)--;
                            // забираем землю из инвентаря
                            (**intBlocks)--;
                        }
                    }
                }
            }
            // если прозрачный режим
            else if (intMode==9){
                // если не дошёл до верхней границы карты
                if(**y>0)(**y)--;
            }
        }
        // ------------ ВНИЗ -----------
        else if(**goWhere==2){
            // если режим копания
            if (intMode==1){
                // если не дошли до границы карты
                if(**y<(yMax-1)){
                    // если снизу земля, то берем блок и идем вниз
                    if((arrWorld[**x][**y+1]=="n")||(arrWorld[**x][**y+1]=="m")){
                        //теперь в клетке воздух
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**y)++;
                        // добавляем землю в инвентарь
                        (**intBlocks)++;
                    } else if (arrWorld[**x][**y+1]=="."){
                        //теперь в клетке воздух
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**y)++;
                    }
                }
            }
            // если режим постройки
            else if (intMode==2){
                // если не дошёл до верхней границы карты
                if(**y<(yMax-1)){
                    // если воздух
                    if(arrWorld[**x][**y+1]=="."){
                        // если инвентарь не пуст
                        if((**intBlocks)>0){
                            //теперь в старой клетке земля
                            arrWorld[**x][**y]="n";
                            arrFirstWorld[**x][**y]="n";
                            // перемещаем наверх
                            (**y)++;
                            // забираем землю из инвентаря
                            (**intBlocks)--;
                        }
                    }
                }
            }
            // если прозрачный режим
            else if (intMode==9){
                // если не дошёл до верхней границы карты
                if(**y<(yMax-1))(**y)++;
            }
        }
        // ------------ ВЛЕВО -----------
        else if(**goWhere==3){
            // режим копания
            if (intMode==1){
                // если не дошёл до границы
                if(**x>0){
                    // если слева земля, собираем блоки и идем налево
                    if((arrWorld[**x-1][**y]=="n")||(arrWorld[**x-1][**y]=="m")){
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**x)--;
                        (**intBlocks)++;
                    }
                    // если слева пусто, просто идем налево
                    else if(arrWorld[**x-1][**y]=="."){
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**x)--;
                    }
                }
            }
            // обычный режим
            else if (intMode==0){
                // если не дошли до границы
                if(**x>0){
                    // если слева пусто
                    if(arrWorld[**x-1][**y]=="."){
                        // закрашиваем старое место - воздух
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        // перемещаемся влево
                        (**x)--;
                        // проверяем, есть ли что-то внизу
                        // если не находимся на нижней границе
                        if(**y<(yMax-1)){
                            // включаем гравитацию
                            // проверяем, есть ли под героем воздух
                            // если есть
                            if(arrWorld[**x][**y+1]=="."){
                                // ищем под ним землю
                                do {
                                    // переходим на клетку с землей
                                    arrFirstWorld[**x][**y]=".";
                                    arrWorld[**x][**y]=".";
                                    (**y)++;
                                // делаем, пока не дойдем до земли
                                // или до границы
                                } while ((arrWorld[**x][**y+1]!="n")&&(arrWorld[**x][**y+1]!="m")&&(**y+1<yMax));
                            }
                        }
                    }
                    // если слева земля, забираемся по стенке
                    else if((arrWorld[**x-1][**y]=="n")||(arrWorld[**x-1][**y]=="m")){
                        // проверка верхней границы
                        if(**y>0){
                            // если над нами земля, идем по потолку
                            if((arrWorld[**x][**y-1]=="n")||(arrWorld[**x][**y-1]=="m")){
                                if((**x)<(xMax-1))(**x)++;
                            }
                            // есть ли над ней два блока: земля+воздух (доработать границы)
                            else if(((arrWorld[**x-1][**y]=="n")||(arrWorld[**x-1][**y]=="m"))&&(arrWorld[**x-1][**y-1]==".")){
                                (**x)--;
                                (**y)--;
                            }
                            // если стена, лезем наверх
                            else {
                                arrFirstWorld[**x][**y]=".";
                                arrWorld[**x][**y]=".";
                                (**y)--;
                            }
                        }
                    }
                }
            }
            // режим постройки
            else if (intMode==2){
                // граница
                if(**x>0){
                    // если слева воздух
                    if(arrWorld[**x-1][**y]=="."){
                        // если инвентарь не пуст
                        if((**intBlocks)>0){
                            //теперь в старой клетке земля
                            arrWorld[**x][**y]="n";
                            arrFirstWorld[**x][**y]="n";
                            // перемещаем наверх
                            (**x)--;
                            // забираем землю из инвентаря
                            (**intBlocks)--;
                        }
                    }
                }
            }
            // прозрачный режим
            else if (intMode==9){
                // если не дошёл до границы
                if(**x>0)(**x)--;
            }
        }
        // ------------ ВПРАВО -----------
        else if(**goWhere==4){
            // режим копания
            if (intMode==1){
                // если не дошёл до границы
                if(**x<(xMax-1)){
                    // если справа земля, берем блок, идем направо
                    if((arrWorld[**x+1][**y]=="n")||(arrWorld[**x+1][**y]=="m")){
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**x)++;
                        (**intBlocks)++;
                        // генерируем карту справа, если герой посередине экрана
                        // genmap(право/лево);
                    }
                    // если справа пусто, просто идем направо
                    else if(arrWorld[**x+1][**y]=="."){
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        (**x)++;
                    }
                }
            }
            // обычный режим
            else if (intMode==0){
                // если не дошли до границы
                if(**x<(xMax-1)){
                    // если справа пусто
                    if(arrWorld[**x+1][**y]=="."){
                        // закрашиваем старое место - воздух
                        arrWorld[**x][**y]=".";
                        arrFirstWorld[**x][**y]=".";
                        // перемещаемся вправо
                        (**x)++;
                        // проверяем, есть ли что-то внизу
                        // если не находимся на нижней границе
                        if(**y<(yMax-1)){
                            // включаем гравитацию
                            // проверяем, есть ли под героем воздух
                            // если есть
                            if(arrWorld[**x][**y+1]=="."){
                                // ищем под ним землю
                                do {
                                    // переходим на клетку с землей
                                    arrFirstWorld[**x][**y]=".";
                                    arrWorld[**x][**y]=".";
                                    (**y)++;
                                // делаем, пока не дойдем до земли
                                // или до границы
                                } while ((arrWorld[**x][**y+1]!="n")&&(arrWorld[**x][**y+1]!="m")&&(**y+1<yMax));
                            }
                        }
                    }
                    // если справа земля, забираемся по стенке
                    else if((arrWorld[**x+1][**y]=="n")||(arrWorld[**x+1][**y]=="m")){
                        // проверка верхней границы
                        if(**y>0){
                            // если над нами земля, идем по потолку
                            if((arrWorld[**x][**y-1]=="n")||(arrWorld[**x][**y-1]=="m")){
                                if((**x)>0)(**x)--;
                            }
                            // есть ли над ней два блока: земля+воздух (доработать границы)
                            else if(((arrWorld[**x+1][**y]=="n")||(arrWorld[**x+1][**y]=="m"))&&(arrWorld[**x+1][**y-1]==".")){
                                (**x)++;
                                (**y)--;
                            }
                            // если там просто стена, лезем наверх
                            else {
                                arrFirstWorld[**x][**y]=".";
                                arrWorld[**x][**y]=".";
                                (**y)--;
                            }
                        }
                    }
                }
            }
            // режим постройки
            else if (intMode==2){
                // граница
                if(**x<(xMax-1)){
                    // если справа воздух
                    if(arrWorld[**x+1][**y]=="."){
                        // если инвентарь не пуст
                        if((**intBlocks)>0){
                            //теперь в старой клетке земля
                            arrWorld[**x][**y]="n";
                            arrFirstWorld[**x][**y]="n";
                            // перемещаем наверх
                            (**x)++;
                            // забираем землю из инвентаря
                            (**intBlocks)--;
                        }
                    }
                }
            }
            // прозрачный режим
            else if (intMode==9){
                // если не дошёл до границы
                if(**x<(xMax-1))(**x)++;
            }
    }
    arrWorld[**x][**y]="0";
    **goWhere=0;
// возвращаемся в RedrawScreen
}
// ------------------ <GET KEY> ----------------------------
char GetKey(int *goWhere){
    int c;
    c = getch();

    // выход
    if(c=='q'){
        return 1;
    }
    // рестарт
    else if(c=='r'){
        return 2;
    }
    //обычный режим
    else if(c=='1'){
        return 6;
    }
    //режим копания
    else if(c=='2'){
        return 3;
    }
    //режим постройки
    else if(c=='3'){
        return 5;
    }
    //прозрачный режим
    else if(c=='9'){
        return 4;
    }
    //перемещение
    else
    {
        if(c=='w')*goWhere=1; // верх
        else if(c=='s')*goWhere=2; // низ
        else if(c=='a')*goWhere=3; // лево
        else if(c=='d')*goWhere=4; // право
        else *goWhere=0; // стоим на месте
        return 0;
    }
}
// ------------------ </GET KEY> ----------------------------

int SkyGen(int xMax, int yMax, int arrWorld[xMax][yMax]){
    int i,j;

    for(j=0;j<yMax;j++){
        for(i=0;i<xMax;i++){
            arrWorld[i][j]=".";
        }
    }
}

int FloorGen(int xMax, int yMax, int arrWorld[xMax][yMax]){
    int i,j;
    int y1,y2;
    int yTop=yMax-10; // верхняя граница земли (относительно зрителя)
    int yBottom=yMax-1; // нижняя граница

    // x1
    j = rand() % 10; // 0123456789
    y1 = yTop + j;

    // рисуем границу
    arrWorld[0][y1]="m";

    // заполняем пустоты
    for(j=(y1+1);j<=yBottom;j++){
            arrWorld[0][j]="n";
    }

    // x2
    for(i=1;i<xMax;i++){
        if((y1>yTop)&&(y1<yBottom)) j = (rand() % 3) - 1; // 012 (-1 0 1)
        else if (y1>=yBottom) j = (rand() % 2) - 1; // 01 (-1 0)
        else if (y1<=yTop) j = rand() % 2; // 0 1
        y2 = y1 + j;
        // рисуем границу
        arrWorld[i][y2]="m";
        // заполняем пустоты
        for(j=y2+1;j<=yBottom;j++)arrWorld[i][j]="n";
        y1 = y2;
    }
}

int HeroGen(int xMax, int yMax, int arrWorld[xMax][yMax], int **x, int **y)
{
    int j;

    (**x)=0;

    for(j=0;j<yMax;j++){
        if(arrWorld[0][j]=="m"){
            (**y)=j-1;
            //arrWorld[0][(**y)]="0";
            break;
        }
    }
// возвращаемся в WorldGen
}

int calcBackgr(int xMax, int yMax, int arrFirstWorld[xMax][yMax], int arrWorld[xMax][yMax]){
    int i,j;

    for(j=0;j<yMax;j++){
        for(i=0;i<xMax;i++){
            arrWorld[i][j]=arrFirstWorld[i][j];
        }
    }
// возврат в RedrawScreen
}


int DrawWorld(int xMax, int yMax, int arrWorld[xMax][yMax]){
    int i,j;

    //printf("WORLD\n");
    for(j=0;j<yMax;j++){
        for(i=0;i<xMax;i++){
            printf("%s", arrWorld[i][j]);
        }
        printf("\n");
    }
}
