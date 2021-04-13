#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#pragma warning(disable:4996)
#define N 200000


void* pointer;

//hlavi�ka za�aitku pola + hlavi�ka volnej �asti pamate
typedef struct Header {
    unsigned   int sizeOfMemory;
    unsigned   int size;
    struct Header* next;
}HEADER;


//hlavi�ka alokovanej �asti pamate
typedef struct HeaderOfAlocated {
    unsigned short int size;
}HEADERA;


void memory_init(void* ptr, unsigned int size) {
    char* zaciatok = (char*)ptr;
    //inicializacia zadanej �asti po�a na -2
    for (int i = 0; i < size; i++, zaciatok++) {
        *zaciatok = -2;
    }
    //vytvorenie hlavnej hlavi�ky
    HEADER* header = (HEADER*)ptr;
    header->size = size - sizeof(HEADER) - sizeof(HEADERA);
    header->sizeOfMemory = size;
    header->next = NULL;

}

int memory_check(void* ptr) {
    char* valid = (char*)ptr;

    if (*(valid) == -1) {
        return 1;
    }
    else {
        return 0;
    }
}

void* memory_alloc(unsigned long size) {
    HEADER* header = (HEADER*)pointer;
    HEADER* curr = header;
    char* start_memory = (char*)pointer;

    //toto je pr�pad, ak sa za hlavnou hlavi�kou nach�dza NULL pointer, �o zna��, �e v poli e�te nie je ni� alokovan�
    if (header->next == NULL) {
        //ak je ve�kos� vo�nej pamate viac alebo sa rovn� ve�kosti pamate, ktor� chceme alokova� -> tak ju alokujeme
        if (header->size >= size) {
            //toto je pr�pad, ak alokujem to�ko pamate, �e neostane miesto pre �al�iu hlavi�ku, ktor� ozna�uje �al�� vo�n� blok
            //tak toto miesto prid�m k alokovanej pamati, aby mi neostalo zbyto�ne nevyu�it� miesto
            if (header->size < size + sizeof(HEADER) + sizeof(HEADERA)) {
                HEADERA* new_headerA = (HEADERA*)(start_memory + sizeof(HEADER));
                new_headerA->size = header->size;
                header->size = 0;
                memset(start_memory + sizeof(HEADER) + sizeof(HEADERA), -1, new_headerA->size);
                return (start_memory + sizeof(HEADER) + sizeof(HEADERA));
            }
            //toto je pr�pad, ak tam je miesto aj na �al�iu hlavi�ku, ktor� ukazuje na �al�ie vo�n� miesto v pamati
            else {
                HEADERA* new_headerA = (HEADERA*)(start_memory + sizeof(HEADER));
                new_headerA->size = size;
                memset(start_memory + sizeof(HEADER) + sizeof(HEADERA), -1, size);
                HEADER* new_headerF = (HEADER*)(start_memory + sizeof(HEADER) + sizeof(HEADERA) + size);
                new_headerF->size = header->size - size - sizeof(HEADERA);
                new_headerF->sizeOfMemory = header->sizeOfMemory;
                new_headerF->next = NULL;
                header->next = new_headerF;
                header->size = 0;
                return (start_memory + sizeof(HEADER) + sizeof(HEADERA));
            }
        }
        //ak nie je dostatok miesta na alokovanie pamate vyp�e sa v�pis a vr�ti sa NULL
        else {
            return NULL;
        }
    }
    //toto else zan�� to, �e u� sa do pamate niekedy nie�o alokovalo 
    else {
        HEADER* previous;
        HEADER* thebest = NULL;

        int minimum = N;    //premenn�, ktor� ozna�uje minim�lny zvy�ok, ktor� sa n�jde, aby ost�vali v�dy �o najmen�ie vo�n� k�sky a vyu�ilo sa �o najviac pam�te 

        //flag mi ozna�uje to, �e mi na�lo nejak� vo�nu �as� pam�te, a ak flag ostane 0 aj po vykonan� while-u, tak to znamen�, �e sa nena�lo 
        //dostato�ne ve�k� miesto na alokovanie danej ve�kosti

        int flag = 0;
        int flag2 = 0;      //pomocn� premenn�
        int count = 0;
        int pozicia = 1;    //premenn� poz�cia mi sl��i na to, aby som zistil poz�ciu bloku, ktor� najviac vyhovuje

        //tento while mi prech�dza v�etky vo�n� pamate, preto�e chcem n�js� take vo�n� miesto 
        //v pamati, ktore bude svojou ve�kos�ou, �o najbli��ie
        //k ve�kosti pamate, ktor� chcem alokova� (the best fit)
        while (curr != NULL) {
            flag2 = 0;
            //je tu break preto, lebo ak sa najde presne tak� vo�n� pama� s presnou ve�kos�ou, nemus�m h�ada� �alej
            if (curr->size == size) {
                count++;
                pozicia = count;
                thebest = curr;
                flag = 1;
                break;
            }
            //podmienka, ktor� mi zabezpe�� to, �e n�jdem tak� �as� volnej pamate, pri ktorej bude najmen�ia fragment�cia
            if (curr->size > size && minimum > curr->size - size) {
                minimum = curr->size - size - 1;
                thebest = curr;
                flag = 1;
                count++;
                pozicia = count;
                flag2 = 1;
            }
            if (!flag2) {
                count++;
            }
            curr = curr->next;
        }

        //ak sa nena�la �iadna pama� s dostato�nou ve�kostou vyp�e sa v�pis a vr�ti sa NULL
        if (!flag) {
            return NULL;
        }

        previous = header;

        //ak bude pozicia = 1 znamen� to, �e najlep�� kandid�t bola pama�, ktor� m� ako hlavi�ku hlavn� hlavi�ku
        if (pozicia == 1) {
            char* curr_memory = (char*)thebest;
            HEADERA* new_nodeA = (HEADERA*)(curr_memory + sizeof(HEADER));

            //t�to podmienka overuje to, �i sa zo zvy�ku, ktor� ostal d� vyu�i� e�te nejak� pama�, ak ostane tak� pama�, ktor� je mal�
            //vst�pi to do "if" a zvy�n� pam� pridel� k alokovanej
            if (previous->size < size + sizeof(HEADER) + sizeof(HEADERA)) {
                new_nodeA->size = previous->size;
                previous->size = 0;
                memset(curr_memory + sizeof(HEADER) + sizeof(HEADERA), -1, new_nodeA->size);
                return (curr_memory + sizeof(HEADER) + sizeof(HEADERA));
            }
            //tu sa d� pama� e�te vyu�i� a preto tu vytv�ram �al�iu hlavi�ku, ktor� bude ukazova� na novovzniknut� vo�n� �as� pamata
            else {
                HEADER* new_nodeF = (HEADER*)(curr_memory + sizeof(HEADER) + sizeof(HEADERA) + size);
                new_nodeF->size = previous->size - size - sizeof(HEADERA);
                new_nodeF->next = previous->next;
                previous->next = new_nodeF;
                memset(curr_memory + sizeof(HEADER) + sizeof(HEADERA), -1, size);
                return (curr_memory + sizeof(HEADER) + sizeof(HEADERA));
            }

        }

        int i = 1;
        //cyklus, kor� sl��i na to, aby som na�iel predch�dzaj�ci blok pam�ta a mohol ho spoji� s nov�m 
        while (i != pozicia - 1) {
            previous = previous->next;
            i++;
        }

        //nejako si potrebujem zabezbe�i� predchodcu pointer curr, preto�e budem curr vymaz�va�, preto�e nebude odkazova� 
        //vo�ne miesto v pam�ti, tak t�m p�dom mi je tento pointer zbyto�n� a m��em ho pou�i� sk�r na alok�ciu pam�te

        if (thebest->size < size + sizeof(HEADER) + sizeof(HEADERA)) {
            char* curr_memory = (char*)thebest;
            int number = thebest->size;
            previous->next = thebest->next;
            HEADERA* new_nodeA = (HEADERA*)curr_memory;
            new_nodeA->size = number;
            memset(curr_memory + sizeof(HEADERA), -1, number);
            return curr_memory + sizeof(HEADERA);
        }

        char* curr_memory = (char*)thebest;

        HEADER* new_nodeF = (HEADER*)(curr_memory + size + sizeof(HEADERA));
        new_nodeF->size = thebest->size - sizeof(HEADERA) - size;
        new_nodeF->next = thebest->next;
        new_nodeF->sizeOfMemory = header->sizeOfMemory;
        previous->next = new_nodeF;

        HEADERA* new_nodeA = (HEADERA*)(curr_memory);
        new_nodeA->size = size;

        memset(curr_memory + sizeof(HEADERA), -1, size * sizeof(char));

        char* returnMemory = (char*)new_nodeA;
        return returnMemory + sizeof(HEADERA);
    }
}


int memory_free(void* valid_ptr) {

    if (!memory_check(valid_ptr)) {
        return 1;
    }
    char* curr_memory = (char*)valid_ptr - sizeof(HEADERA);
    HEADERA* helpMemory = (HEADERA*)curr_memory;
    int sizeOfMemory = helpMemory->size;

    //ak sa do funkcie free po�le NULL pointer

    char* header = (char*)pointer;
    HEADER* hlavicka = (HEADER*)pointer;
    //toto je pr�pad, ke� uvol�ujem tak� pama�, ktor� v�avo sused� s hlavnou hlavi�kou, ktor� ukazuje na za�iatok cel�ho po�a
    //m��u nasta� 3 pr�pady:

    if (curr_memory == (header + sizeof(HEADER))) {

        //toto je pr�pad ak je v pamati alokovana jedna ve�k� pama�, ktor� ide a� po koniec celeho pola
        if ((curr_memory + sizeof(HEADERA) + sizeOfMemory) == (header + hlavicka->sizeOfMemory)) {
            hlavicka->size += sizeOfMemory;
            memset(curr_memory, -2, sizeOfMemory + sizeof(HEADERA));
            return 0;
        }

        //druh� pr�pad je, �e vpravo od nej je pama�, ktor� je alokovan� => zv��im ve�kos� vo�nej pamate v hlavnej hlavi�ke
        else if (*(curr_memory + sizeOfMemory + 2 * sizeof(HEADERA)) == -1) {
            hlavicka->size += sizeOfMemory;
            memset(curr_memory, -2, sizeOfMemory + sizeof(HEADERA));
            return 0;
        }

        //tret� pr�pad je, �e vpravo od nej sa nach�dza pama�, ktor� je taktie� vo�n� => spoj�m v�etky bloky pamate
        else if (*(curr_memory + sizeOfMemory + sizeof(HEADERA) + sizeof(HEADER)) == -2) {
            HEADER* remove_memory = (HEADER*)(curr_memory + sizeof(HEADERA) + sizeOfMemory);
            hlavicka->size += sizeOfMemory + sizeof(HEADERA) + remove_memory->size;
            hlavicka->next = remove_memory->next;
            memset(curr_memory, -2, sizeof(HEADER) + sizeOfMemory + sizeof(HEADERA));
            return 0;
        }
        else {
            return 1;
        }
    }
    //pr�pad ak pred danou pam�ou je taktie� vo�n� pam� 
    else if (*(curr_memory - 1) == -2) {
        //pr�pad ak pred danou pama�ou je vo�n� pama� a z�roven za danou pama�ou je hlavi�ka, ktor� ukazuje na �al�iu vo�n� pama�
        //rob�m to preto, aby som vybral tak� blok pamate, s ktor�m spojim danu pama�, aby v�sledn� pama� bola �o najva�ia


        //ak je toto if pravdiv� znamen� to �e dan� pama�, ktoru chcem uvolni� je ohrani�en� dvoma dalsimi volnymi pamatami -> ze spojim tieto 3 bloky pamate
        //do jednej velkej
        if (*(curr_memory + sizeof(HEADERA) + sizeOfMemory + sizeof(HEADER)) == -2 &&
            curr_memory + sizeof(HEADERA) + sizeOfMemory != header + hlavicka->sizeOfMemory) {

            HEADER* next = (HEADER*)(curr_memory + sizeof(HEADERA) + sizeOfMemory);
            HEADER* previous = (HEADER*)pointer;

            while (previous->next != next) {
                previous = previous->next;
            }

            previous->size += next->size + sizeOfMemory + 2 * sizeof(HEADERA);
            previous->next = next->next;

            curr_memory = (char*)previous + sizeof(HEADER);

            memset(curr_memory, -2, previous->size + sizeof(HEADERA));

            return 1;
        }
        //toto je pr�pad, ak je pred pam�ou vo�n� pama� a za pama�ou nie -> tak�e spoj�m tieto dve vo�n� pamate do jednej v��ej
        else {
            int index = 1;
            while (*(curr_memory - index) == -2) {
                index++;
            }
            index--;

            //budem h�ada� index tej hlavi�ky ktor� je najbli��ia tejto vo�nwj pamate

            HEADER* new_header = (HEADER*)(curr_memory - sizeof(HEADER) - index);
            new_header->size += sizeOfMemory + sizeof(HEADERA);

            curr_memory = (char*)valid_ptr;
            memset(curr_memory - sizeof(HEADERA), -2, sizeOfMemory + sizeof(HEADERA));

            return 1;
        }
    }
    //pr�pad, ktor� hovor� to, �e uvolnujem tak� pamat, ktor� je na konci celeho pola a pred sebou m� alokovanu pamat
    else if (curr_memory + sizeof(HEADERA) + helpMemory->size == header + hlavicka->sizeOfMemory) {
        int index = 1;

        //cyklus mi h�ad� najbli�iu hlavi�ku vo�nej pamate, tam nie je tak n�jde hlavi�ku, ktor� ukazuje na za�iatok celeho pola
        while (1) {
            if (*(curr_memory - index) != -1) {
                if (*(curr_memory - index - sizeof(HEADERA)) == -1) { //podmienky pomocou ktor�ch kontrolujem �i sa na danej adrese nenachadza alokovana �as�
                    if (*(curr_memory - index - 2 * sizeof(HEADERA)) == -1) {
                        index += 2 * sizeof(HEADERA);
                        continue;
                    }
                    index++;
                    break;
                }
                index++;
                break;
            }
            index++;
        }
        //n�jdenie predch�dzaj�cej hlavi�ky, aby som ju mohol spoji� s novou
        HEADER* previous = (HEADER*)(curr_memory - index - sizeof(HEADER));
        int size = helpMemory->size;
        //nov� hlavi�ka
        HEADER* new_header = (HEADER*)(curr_memory);
        new_header->size = size;
        new_header->sizeOfMemory = previous->sizeOfMemory;
        new_header->next = previous->next;
        previous->next = new_header;

        memset(curr_memory + sizeof(HEADER), -2, size + sizeof(HEADERA) - sizeof(HEADER));

        return 1;
    }
    //pr�pad ak pred danou pama�ou nie je vo�n� pama� ale alokovan� a za danou pam�ou sa nach�dza hlavi�ka vo�n�ho bloku
    //DONE !!!!!!!
    else if (*(curr_memory + sizeof(HEADERA) + sizeOfMemory + sizeof(HEADER)) == -2) {
        HEADER* next = (HEADER*)(curr_memory + sizeof(HEADERA) + sizeOfMemory);
        HEADER* new_header = (HEADER*)(curr_memory);
        HEADER* previous = (HEADER*)pointer;
        while (previous->next != next) {
            previous = previous->next;
        }

        new_header->sizeOfMemory = previous->sizeOfMemory;
        new_header->size = sizeOfMemory + sizeof(HEADERA) + next->size;
        new_header->next = next->next;

        previous->next = new_header;
        memset(curr_memory + sizeof(HEADER), -2, sizeof(HEADER) + sizeof(HEADERA));

        return 1;
    }
    //pr�pad ke� pred pamatou je alokovan� aj za pama�ou je alokoivana pama�
    //OKOMENTOVAT DONE!!!
    else {
        int index = 1;

        //cyklus mi h�ad� najbli�iu hlavi�ku vo�nej pamate, tam nie je tak n�jde hlavi�ku, ktor� ukazuje na za�iatok celeho pola
        while (1) {
            if (*(curr_memory - index) != -1) {
                if (*(curr_memory - index - sizeof(HEADERA)) == -1) {
                    if (*(curr_memory - index - 2 * sizeof(HEADERA)) == -1) {
                        index += 2 * sizeof(HEADERA);
                        continue;
                    }
                    index++;
                    break;
                }
                index++;
                break;
            }
            index++;
        }

        //n�jdenie predch�dzaj�cej hlavi�ky, aby som ju mohol spoji� s novou
        HEADER* previous = (HEADER*)(curr_memory - index - sizeof(HEADER));
        int size = helpMemory->size;
        //nov� hlavi�ka
        HEADER* new_header = (HEADER*)(curr_memory);
        new_header->size = size;
        new_header->sizeOfMemory = previous->sizeOfMemory;
        new_header->next = previous->next;
        previous->next = new_header;

        memset(curr_memory + sizeof(HEADER), -2, size + sizeof(HEADERA) - sizeof(HEADER));

        return 1;
    }


    return 0;
}

//funkcia, ktora slu�i na spo��tanie alokovanej �asti pam�te
void memory_count(void* valid_ptr, int size, int sizeOfAlloc) {
    char* start = (char*)valid_ptr + sizeof(HEADER);
    HEADERA* sizeOfAllocated = (HEADERA*)start;
    int count = sizeof(HEADER);
    int countOfHeaders = 0;
    int curr_memory = sizeof(HEADER);
    int temp;
    printf("\tAlokacia velkosti %dB pri velkosti pamate %dB.\n\n", sizeOfAlloc, size);
    //cyklus bude prebiaha� dovtedy, k�m sa neprejde cel� pole
    while (count != size) {
        //cyklus mo�e zasta� ak sa na danej adrese nenach�dza -1, �o znamen�, �e sa tam nach�dza bude vo�na pama� alebo nejak� hlavi�ka vo�neho bloku
        if (*((char*)sizeOfAllocated + sizeof(HEADERA)) != -1 || *((char*)sizeOfAllocated + sizeof(HEADERA) + 1) != -1 && *((char*)sizeOfAllocated + sizeof(HEADERA) + 2) != -1) {
            break;
        }
        count += sizeOfAllocated->size + sizeof(HEADERA);
        temp = sizeOfAllocated->size;
        countOfHeaders++;
        start = (char*)(sizeOfAllocated);
        sizeOfAllocated = (HEADERA*)(start + temp + sizeof(HEADERA));
    }

    count = count - (countOfHeaders * sizeof(HEADERA)) - sizeof(HEADER);
    double percent = (100 * count) / size;
    printf("\tZ celkoveho bloku pamate velkosti %dB je alokovanych %dB, co je %.2f %% z celkovej velkosti.\n\n\n", size, count, percent);

}

//zhodn� funkcia ako memory_count1, len je to vytvorene pre testy, kde sa hodnoty ud�vaj�
void memory_count2(void* valid_ptr, int size, int from, int to) {
    char* start = (char*)valid_ptr + sizeof(HEADER);
    HEADERA* sizeOfAllocated = (HEADERA*)start;
    int count = sizeof(HEADER);
    int countOfHeaders = 0;
    int curr_memory = sizeof(HEADER);
    int temp;
    printf("\tAlokacia velkosti od %dB do %dB pri velkosti pamate %dB.\n\n", from, to, size);
    while (count != size) {
        if (*((char*)sizeOfAllocated + sizeof(HEADERA)) != -1 || *((char*)sizeOfAllocated + sizeof(HEADERA) + 1) != -1 && *((char*)sizeOfAllocated + sizeof(HEADERA) + 2) != -1) {
            break;
        }
        count += sizeOfAllocated->size + sizeof(HEADERA);
        temp = sizeOfAllocated->size;
        countOfHeaders++;
        start = (char*)(sizeOfAllocated);
        sizeOfAllocated = (HEADERA*)(start + temp + sizeof(HEADERA));
    }

    count = count - (countOfHeaders * sizeof(HEADERA)) - sizeof(HEADER);
    double percent = (100 * count) / size;
    printf("\tZ celkoveho bloku pamate velkosti %dB je alokovanych %dB, co je %.2f %% z celkovej velkosti.\n\n\n", size, count, percent);
}

//testova� pre test 1
//odsk��am tu v�etky funkcie: memory_alloc, memory_free, memory_check aj memory_init
void test1(int size, void* arrayOfPointers[], int sizeOfAlloc) {
    int index = 0;
    memory_init(pointer, size);
    //iterujem a� dovtedy, k�m mi funkcia malloc nevr�ti NULL �o znamen�, �e u� tam nie je miesto na alok�ciu danej ve�kosti pam�te
    while (1) {
        arrayOfPointers[index] = memory_alloc(sizeOfAlloc);
        if (arrayOfPointers[index] == NULL) {
            break;
        }
        index++;
    }
    //n�sledne sa zr�ta % alokovanej pam�te
    memory_count(pointer, size, sizeOfAlloc);
    //uvo�nia sa jednotlive bloky pam�te
    for (int i = 0; i < index; i++) {
        memory_free(arrayOfPointers[i]);
    }

}
//testova� na testy 2,3,4
//podobn� funkcia ako na test 1, len s t�m rozdielom, �e sa vo funkci� generuj� n�hodne ve�kosti pam�te, ktor� budem alokova�
void test234(int size, void* arrayOfPointers[], int from, int to) {
    HEADER* header;
    HEADERA* headera;
    char* curr;
    int index = 0;
    int random;
    int arr[10000];
    srand(time(0));
    random = (rand() % (to - from + 1)) + from;
    arr[0] = random;
    memory_init(pointer, size);
    //generovanie n�hodn�ch ve�kost� pam�te m�m vyrie�en� tak, �e
    while (1) {
        arrayOfPointers[index] = memory_alloc(random);
        if (arrayOfPointers[index] == NULL) {
            break;
        }
        curr = (char*)arrayOfPointers[index] - sizeof(HEADERA);
        headera = (HEADERA*)curr;
        curr = (char*)curr + headera->size + sizeof(HEADERA);
        header = (HEADER*)curr;
        if (header->size > to) {
            random = (rand() % (to - from + 1)) + from;
        }
        else if (header->size == from) {
            random = from;
        }
        else {
            random = (rand() % (header->size - from + 1)) + from;
        }
        index++;
        arr[index] = random;
    }
    memory_count2(pointer, size, from, to);
    for (int i = 0; i < index; i++) {
        memory_free(arrayOfPointers[i]);
    }
    printf("\n");
}


int main() {
    char pole[N] = { 0 };
    pointer = pole;
    int flag = 1;
    int cisloTestu;
    void* arrayOfPointers[60000] = { 0 };
    int index = 0;
    int size = 50;
    int random;
    int random2;

    printf("Testovacie scenare:\n");
    printf("Test 1: Alokacia jednej nahodnej hodnoty od 8-24 byte do inicializovanej pamate velkosti do 50/100/200 byte do zaplnenia pamate.\n");
    printf("Test 2: Alokacia nahodneho poctu byte (8 - 24) do inicializovanej pamate velkosti do 50/100/200 byte do zaplnenia pamate.\n");
    printf("Test 3: Alokacia nahodneho poctu byte (500 - 5 000) byte do inicializovanej pamate velkosti do 10 000 byte do zaplnenia pamate.\n");
    printf("Test 4: Alokacia nahodneho poctu byte (8 - 50 000) byte do inicializovanej pamate velkosti do 65 000 byte do zaplnenia pamate.\n");
    printf("Pre ukoncenie stlacte 0.\n");
    while (flag) {
        printf("Pre spustenie daneho testu stlacte prislusne cislo.\n");
        scanf("%d", &cisloTestu);
        switch (cisloTestu) {
        case 1:
            size = 50;
            srand(time(0));
            random2 = (rand() % (24 - 8 + 1)) + 8;
            for (int j = 0; j < 3; j++) {
                test1(size, arrayOfPointers, random2);
                size += size;
            }
            break;
        case 2:
            size = 50;
            for (int j = 0; j < 3; j++) {
                test234(size, arrayOfPointers, 8, 24);
                size += size;
            }
            break;
        case 3:
            srand(time(0));
            random = (rand() % (10000 - 5000 + 1)) + 5000;
            test234(random, arrayOfPointers, 500, 5000);
            break;
        case 4:
            srand(time(0));
            random = (rand() % (100000 - 50000 + 1)) + 50000;
            test234(random, arrayOfPointers, 8, 50000);
            break;
        case 0:
            flag = 0;
            break;
        default:
            printf("Nespravne cislo testu.\n");
            break;
        }
    }
    return 0;

}
