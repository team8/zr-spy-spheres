//Begin page CONSTANTS
#define MIRROR_NUM 2 //numero di item mirror presenti nel game
#define SCORE_NUM 4 //numero di item score presenti nel game
#define ENERGY_NUM 3 //numero di item energy presenti nel game
#define ITEM_NUM 9 //numero totale di item
#define MEMORY_MAX 2 //numero massimo di foto scattabili
#define MAX_TIME 186 //durata dell'intero match in secondi

#define MIRROR_TIME 24 //durata in secondi del mirror

#define MIN_PIC_DIST 0.5 //distanza minima per eseguire una foto
/*
#define zoneX 0.64      //limiti campo +-valore X
#define zoneY 0.8       //limiti campo +-valore Y
#define zoneZ 0.64      //limiti campo +-valore Z
*/

#define UPLOAD_MAX_ATT_VEL 0.05f //massima velocita' angolare per eseguire l'upload
#define MAX_LOOK_ANGLE 0.968912f //massimo errore per guardare qualcosa

float EARTH[3]; //punto da guardare per l'upload

float ENERGY[ENERGY_NUM][3]; //contiene le coordinate item energy
float SCORE[SCORE_NUM][3]; //contiene le coordinate item score
float MIRROR[MIRROR_NUM][3]; //contiene le coordinate item mirror

//End page CONSTANTS
//Begin page DISTANCE
float between[3];

/*
    calcola la distanza tra 2 punti
*/

float distance(float p1[], float p2[])
{
    mathVecSubtract(between, p1, p2, 3);
    return mathVecMagnitude(between, 3);
}
//End page DISTANCE
//Begin page ENEMY_USED_MIRROR
char timeUsedMirror; //contiene il momento nel quale ci siamo accorti che l'avversario sta usando il mirror
char enemyMirror; //mirror a disposizione dell'avversario
char enemyMirrorUsed; //mirror usati dall'avversario

/*
    retituisce true se l'avversario sta usando il mirror, consuma meno energia possibile
*/

bool enemyUsedMirror(bool controlledPic = false) //controlledPic = true se si puo' usare game.getPicPoints() cioe' se ci sono le condizioni per effettuare una foto
{

    //conta quanti mirror ha preso l'avversario
    enemyMirror = 0;
    if (info.item.mirror[0] == 1)
        enemyMirror++;
    if (info.item.mirror[1] == 1)
        enemyMirror++;
    if (enemyMirror == 0)
        return false; //se non ne ha preso neanche 1 restituisce false
    //------------------------------------------

    //se l'avversario ha usato il mirror verifica se e' ancora attivo
    if (enemyMirrorUsed > 0) {
        if ((info.elapsedTime - timeUsedMirror) <= MIRROR_TIME)
            return true; //se e' ancora attivo restituisce true
        if ((enemyMirror - enemyMirrorUsed) == 0)
            return false; //se non ha più mirror a disposizione restituisce false
    }
    //--------------------------------------------------------------------

    //simula un tentativo di foto, se fallisce per negativo, allora l'avversario ha attivato il mirror
    if (controlledPic && game.getPicPoints() < 0.0f) {
        timeUsedMirror = info.elapsedTime;
        enemyMirrorUsed++;
        return true;
    }
    //----------------------------------------------------------------------
    return false;
}

//End page ENEMY_USED_MIRROR
//Begin page INFO
struct object {
    char energy[ENERGY_NUM]; //stato items energy                --> se = 0 hai preso tu
    char score[SCORE_NUM]; //stato items score                 --> se = 1 preso avversario
    char mirror[MIRROR_NUM]; //stato items mirror                --> se = -1 ancora in game
};

struct data {
    float myState[12], enemyState[12]; //stato dei 2 spheres
    float myPos[3], enemyPos[3]; //posizioni dei 2 spheres
    float myAtt[3], enemyAtt[3]; //attitudine dei 2 spheres
    float myAngularVel[3]; //velocita' angolare del mio sphere
    char myDial; //indica in quale quadrante si trova -1 = dark \ 0 = grey \ 1 = light
    char enemyDial; //indica in quale quadrante si trova -1 = dark \ 0 = grey \ 1 = light
    int fuel; //fuel restante
    char myEnergy, enemyEnergy; //energia restante dei 2 spheres
    char myScore, enemyScore; //punteggio dei 2 spheres
    char usedMemory; //quantita di memoria usata
    char elapsedTime; //tempo trascorso dall'inizio del match
    object item; //item nel gioco
} info;

void update()
{
    //aggiornamento dello stato
    api.getMyZRState(info.myState);
    api.getOtherZRState(info.enemyState);

    //isolamento della posizione
    for (int i = 0; i < 3; i++) {
        info.myPos[i] = info.myState[i];
        info.enemyPos[i] = info.enemyState[i];
    }

    //isolamento dell' attitudine
    for (int i = 6; i < 9; i++) {
        info.myAtt[i - 6] = info.myState[i];
        info.enemyAtt[i - 6] = info.enemyState[i];
    }

    //isolamento della velocita' angolare
    for (int i = 9; i < 12; i++) {
        info.myAngularVel[i - 9] = info.myState[i];
    }

    //definizione del quadrante (dark, grey, light)
    info.myDial = game.posInArea(info.myPos);
    info.enemyDial = game.posInArea(info.enemyPos);

    //aggiornamento fuel
    info.fuel = game.getFuelRemaining();

    //aggiornamento energia
    info.myEnergy = game.getEnergy();
    info.enemyEnergy = game.getOtherEnergy();

    //aggiornamento score
    info.myScore = game.getScore();
    info.enemyScore = game.getOtherScore();

    //aggiornamento memoria
    info.usedMemory = game.getMemoryFilled();

    //aggiornamento tempi
    info.elapsedTime = game.getCurrentTime();

    //aggiornamento posizione "earth" per l'upload
    EARTH[0] = info.myPos[0];
    EARTH[1] = info.myPos[1];

    //aggiornamento stato items
    for (int id = 0; id < ITEM_NUM; id++) {
        if (id < ENERGY_NUM)
            info.item.energy[id] = game.hasItem(id);
        else if (id < ENERGY_NUM + SCORE_NUM)
            info.item.score[id - ENERGY_NUM] = game.hasItem(id);
        else if (id < ENERGY_NUM + SCORE_NUM + MIRROR_NUM)
            info.item.mirror[id - (ENERGY_NUM + SCORE_NUM)] = game.hasItem(id);
    }
}
//End page INFO
//Begin page IS_FACING
float relativePos[3];

/*
    retituisce vero se lo spheres indicato dall'ID sta guardando il punto indicato in POS 
    ID = 0 -> noi
    ID = 1 -> avversario
*/

bool isFacing(float pos[], char id)
{
    if (id == 0) {
        mathVecSubtract(relativePos, pos, info.myPos, 3);
        mathVecNormalize(relativePos, 3);
        if (mathVecInner(info.myAtt, relativePos, 3) > MAX_LOOK_ANGLE)
            return true;
        else
            return false;
    }
    else {
        mathVecSubtract(relativePos, pos, info.enemyPos, 3);
        mathVecNormalize(relativePos, 3);
        if (mathVecInner(info.enemyAtt, relativePos, 3) > MAX_LOOK_ANGLE)
            return true;
        else
            return false;
    }
}
//End page IS_FACING
//Begin page MOVE_LOOK
float relativeAtt[3];

/*
    si ocupa dell'attitudine e della posizione dello sphere
*/

void move(float point[])
{
    if (info.myEnergy < 0.5)
        stop();
    api.setPositionTarget(point);
}

void look(float point[])
{
    mathVecSubtract(relativeAtt, point, info.myPos, 3);
    mathVecMagnitude(relativeAtt, 3);
    api.setAttitudeTarget(relativeAtt);
}

void stop()
{
    api.setPositionTarget(info.myPos);
}
//End page MOVE_LOOK
//Begin page NEXT
float dist[3], distNear[3];
float item[3];

char early; //contiene l'id dell'item della categoria indicata più vicino

char idNum;

/*
    restittuisce l'ID dell'item piu' vicino della categoria indicata(energy id = 0, score id = 1, mirror id = 2)
    se non ci sono più item della categoria specificata restituisce -1
*/

char next(char id)
{
    if (id == 0)
        idNum = ENERGY_NUM;
    else if (id == 1) {
        id = ENERGY_NUM;
        idNum = SCORE_NUM + ENERGY_NUM;
    }
    else {
        id = SCORE_NUM + ENERGY_NUM;
        idNum = MIRROR_NUM + SCORE_NUM + ENERGY_NUM;
    }

    early = id;

    while (game.hasItem(early) != -1) {
        early++;
        if (early >= idNum)
            return -1;
    }

    for (int i = id; i < idNum; i++) {
        if (game.hasItem(i) == -1) {
            game.getItemLoc(item, i);
            mathVecSubtract(dist, item, info.myPos, 3);

            game.getItemLoc(item, early);
            mathVecSubtract(distNear, item, info.myPos, 3);

            if (mathVecMagnitude(dist, 3) < mathVecMagnitude(distNear, 3))
                early = i;
        }
    }

    return early - id;
}

/*
int next()
{
    early = 0;
            while(info.item.mirror[early] != -1)
            {
                early++;
                if(early >= MIRROR_NUM) return -1;
            }
            
            for(int i = 0; i < MIRROR_NUM; i++)
            {
                if(info.item.mirror[i] == -1)
                {
                    memcpy(item, MIRROR[i], 3 * sizeof(float));
                    mathVecSubtract(dist, item, info.myPos, 3);
                
                    memcpy(item, MIRROR[early], 3 * sizeof(float));
                    mathVecSubtract(distNear, item, info.myPos, 3);
                
                    if(mathVecMagnitude(dist, 3) < mathVecMagnitude(distNear, 3)) early = i;
                }
            }
            return early;
}

*/

/*
char next(char id)
{
    switch(id)
    {
        case 0:
            
            early = 0;
            while(info.item.energy[early] != -1)
            {
                early++;
                if(early >= ENERGY_NUM) return -1;
            }
            
            for(int i = 0; i < ENERGY_NUM; i++)
            {
                if(info.item.energy[i] == -1)
                {
                    memcpy(item, ENERGY[i], 3 * sizeof(float));
                    mathVecSubtract(dist, item, info.myPos, 3);
                
                    memcpy(item, ENERGY[early], 3 * sizeof(float));
                    mathVecSubtract(distNear, item, info.myPos, 3);
                
                    if(mathVecMagnitude(dist, 3) < mathVecMagnitude(distNear, 3)) early = i;
                }
            }
            return early;
        case 1:
            
            early = 0;
            while(info.item.score[early] != -1)
            {
                early++;
                if(early >= SCORE_NUM) return -1;
            }
            
            for(int i = 0; i < SCORE_NUM; i++)
            {
                if(info.item.energy[i] == -1)
                {
                    memcpy(item, SCORE[i], 3 * sizeof(float));
                    mathVecSubtract(dist, item, info.myPos, 3);
                
                    memcpy(item, SCORE[early], 3 * sizeof(float));
                    mathVecSubtract(distNear, item, info.myPos, 3);
                
                    if(mathVecMagnitude(dist, 3) < mathVecMagnitude(distNear, 3)) early = i;
            
                }
            }
            return early;
        case 2:
            
            early = 0;
            while(info.item.mirror[early] != -1)
            {
                early++;
                if(early >= MIRROR_NUM) return -1;
            }
            
            for(int i = 0; i < MIRROR_NUM; i++)
            {
                if(info.item.mirror[i] == -1)
                {
                    memcpy(item, MIRROR[i], 3 * sizeof(float));
                    mathVecSubtract(dist, item, info.myPos, 3);
                
                    memcpy(item, MIRROR[early], 3 * sizeof(float));
                    mathVecSubtract(distNear, item, info.myPos, 3);
                
                    if(mathVecMagnitude(dist, 3) < mathVecMagnitude(distNear, 3)) early = i;
                }
            }
            return early;
    }
    return -1;
}
*/
//End page NEXT
//Begin page PIC_UPLOAD
/*
    si occupa di scattare le foto e eseguire l'upload quando necessario
*/

int pic() //scatta la foto se ci sono tutte le condizioni necessarie
{
    //condizioni rispetto la straregia adottata
    if (needUpload())
        return upload(); //esegue l'upload se necessario
    else
        look(info.enemyPos); //altrimenti punta l'altro spheres

    if (info.myEnergy < 1.5 /*2*/)
        return -2; //verifica se abbiamo sufficente energia per fare la foto
    //----------------------------------------------------------

    //condizioni obbligatorie per il successo della foto
    if (game.getMirrorTimeRemaining() != 0)
        return -3; //verifica che non stiamo usando il mirror
    if (!game.isCameraOn())
        return -4; //verifica se la camera è ativa
    if (!isFacing(info.enemyPos, 0))
        return -5; //verifica se stiamo puntando l'avversario
    if (game.posInDark(info.enemyPos))
        return -6; //verifica se l'avversario sitrova in una zona dove si può fotografare
    if (distance(info.myPos, info.enemyPos) >= MIN_PIC_DIST)
        if (enemyUsedMirror(true))
            return -7; //verifica se l'avversario non sta usando il mirror
    //----------------------------------------------------------

    game.takePic();
    return 1;
}

bool upload() //esegue l'upload
{
    //    if(info.myEnergy < 1.4f) return -1;
    if (isFacing(EARTH, 0)) //controlla se sta guardando la terra
        if (mathVecMagnitude(info.myAngularVel, 3) <= UPLOAD_MAX_ATT_VEL) //verifica se la velocita' consente l'upload
        {
            if (info.myEnergy > 1.1f) {
                game.uploadPics();
                return 2;
            }
        }

    look(EARTH);
    return 0;
}

bool needUpload() //verifica se c'e' bisognio dell'upload o meno
{
    if (info.usedMemory < MEMORY_MAX) {
        //controllo del tempo mancante per la fine dell gara per l'upload finale
        if (info.usedMemory > 0)
            if (info.elapsedTime > MAX_TIME - 20)
                return true;

        return false;
    }
    else
        return true;
}

//End page PIC_UPLOAD
//Begin page SETUP
void setup() //inizializza variabili
{
    //PAGE: ENEMY_USED_MIRROR
    timeUsedMirror = 0;
    enemyMirrorUsed = 0;
    //-------------------------------

    //PAGE: STRATEGY
    //posizione predefinita
    point[0] = 0.0f;
    point[1] = 0.0f;
    point[2] = -0.6f;

    strategyID = 0;
    itemID = 0;
    //-------------------------------

    //PAGE: CONSTANTS
    EARTH[0] = EARTH[1] = 0.0f;
    EARTH[2] = 1.0f;

    for (int id = 0; id < ITEM_NUM; id++) {
        if (id < ENERGY_NUM)
            game.getItemLoc(ENERGY[id], id);
        else if (id < ENERGY_NUM + SCORE_NUM)
            game.getItemLoc(SCORE[id - ENERGY_NUM], id);
        else if (id < ENERGY_NUM + SCORE_NUM + MIRROR_NUM)
            game.getItemLoc(MIRROR[id - (ENERGY_NUM + SCORE_NUM)], id);
    }
    //-------------------------------
}
//End page SETUP
//Begin page STRATEGY
char strategyID; //indica la strategia in atto

char itemID; //id dell'item da prendere

float point[3]; //luogo dove dirigersi

void strategy()
{
    switch (strategyID) {
    case 0: //prende l'id del mirror piu' vicino
        itemID = next(2);
        strategyID++;
        break;
    case 1: //prende il mirror indicato dalla fase precedente
        if (info.item.mirror[itemID] != -1)
            strategyID++;
        move(MIRROR[itemID]);
        break;
    case 2:
        shield();
        itemID = next(2);
        if (itemID == -1)
            strategyID++;
        else
            move(MIRROR[itemID]);
        break;
    case 3: //si mette nella posizione indicata in: "pos" e usa il mirror se serve
        shield();
        move(point);
        break;
    }
}

bool shield()
{
    if (game.getNumMirrorsHeld() > 0) {
        if (needUpload() || info.enemyDial == -1) {
            if (isFacing(info.myPos, 1)) {
                if (info.myDial != -1) {
                    if (info.enemyEnergy > 1 || info.enemyDial == 1) {
                        if (!enemyUsedMirror()) {
                            game.useMirror();
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
//End page STRATEGY
//Begin page main
#include <ZRGame.h>

float debug;

void init()
{
    setup();
}

void loop()
{
    update(); //aggiorna lo stato delle variabili generali

    strategy();

    pic(); //si occupa di scattare le foto e uploadarle se necessario
}
//End page main
