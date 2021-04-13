#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#pragma warning(disable:4996)
#define N 200000


void* pointer;

//hlavièka zaèaitku pola + hlavièka volnej èasti pamate
typedef struct Header {
    unsigned   int sizeOfMemory;
    unsigned   int size;
    struct Header* next;
}HEADER;


//hlavièka alokovanej èasti pamate
typedef struct HeaderOfAlocated {
    unsigned short int size;
}HEADERA;


void memory_init(void* ptr, unsigned int size) {
    char* zaciatok = (char*)ptr;
    //inicializacia zadanej èasti po¾a na -2
    for (int i = 0; i < size; i++, zaciatok++) {
        *zaciatok = -2;
    }
    //vytvorenie hlavnej hlavièky
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

    //toto je prípad, ak sa za hlavnou hlavièkou nachádza NULL pointer, èo znaèí, že v poli ešte nie je niè alokované
    if (header->next == NULL) {
        //ak je ve¾kos vo¾nej pamate viac alebo sa rovná ve¾kosti pamate, ktorú chceme alokova -> tak ju alokujeme
        if (header->size >= size) {
            //toto je prípad, ak alokujem to¾ko pamate, že neostane miesto pre ïalšiu hlavièku, ktorá oznaèuje ïalší vo¾ný blok
            //tak toto miesto pridám k alokovanej pamati, aby mi neostalo zbytoène nevyužité miesto
            if (header->size < size + sizeof(HEADER) + sizeof(HEADERA)) {
                HEADERA* new_headerA = (HEADERA*)(start_memory + sizeof(HEADER));
                new_headerA->size = header->size;
                header->size = 0;
                memset(start_memory + sizeof(HEADER) + sizeof(HEADERA), -1, new_headerA->size);
                return (start_memory + sizeof(HEADER) + sizeof(HEADERA));
            }
            //toto je prípad, ak tam je miesto aj na ïalšiu hlavièku, ktorá ukazuje na ïalšie vo¾né miesto v pamati
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
        //ak nie je dostatok miesta na alokovanie pamate vypíše sa výpis a vráti sa NULL
        else {
            return NULL;
        }
    }
    //toto else zanèí to, že už sa do pamate niekedy nieèo alokovalo 
    else {
        HEADER* previous;
        HEADER* thebest = NULL;

        int minimum = N;    //premenná, ktorá oznaèuje minimálny zvyšok, ktorý sa nájde, aby ostávali vždy èo najmenšie vo¾né kúsky a využilo sa èo najviac pamäte 

        //flag mi oznaèuje to, že mi našlo nejakú vo¾nu èas pamäte, a ak flag ostane 0 aj po vykonaní while-u, tak to znamená, že sa nenašlo 
        //dostatoène ve¾ké miesto na alokovanie danej ve¾kosti

        int flag = 0;
        int flag2 = 0;      //pomocná premenná
        int count = 0;
        int pozicia = 1;    //premenná pozícia mi slúži na to, aby som zistil pozíciu bloku, ktorý najviac vyhovuje

        //tento while mi prechádza všetky vo¾né pamate, pretože chcem nájs take vo¾né miesto 
        //v pamati, ktore bude svojou ve¾kosou, èo najbližšie
        //k ve¾kosti pamate, ktorú chcem alokova (the best fit)
        while (curr != NULL) {
            flag2 = 0;
            //je tu break preto, lebo ak sa najde presne taká vo¾ná pama s presnou ve¾kosou, nemusím h¾ada ïalej
            if (curr->size == size) {
                count++;
                pozicia = count;
                thebest = curr;
                flag = 1;
                break;
            }
            //podmienka, ktorá mi zabezpeèí to, že nájdem takú èas volnej pamate, pri ktorej bude najmenšia fragmentácia
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

        //ak sa nenašla žiadna pama s dostatoènou ve¾kostou vypíše sa výpis a vráti sa NULL
        if (!flag) {
            return NULL;
        }

        previous = header;

        //ak bude pozicia = 1 znamená to, že najlepší kandidát bola pama, ktorá má ako hlavièku hlavnú hlavièku
        if (pozicia == 1) {
            char* curr_memory = (char*)thebest;
            HEADERA* new_nodeA = (HEADERA*)(curr_memory + sizeof(HEADER));

            //táto podmienka overuje to, èi sa zo zvyšku, ktorý ostal dá využi ešte nejaká pama, ak ostane taká pama, ktorá je malá
            //vstúpi to do "if" a zvyšnú pamä pridelí k alokovanej
            if (previous->size < size + sizeof(HEADER) + sizeof(HEADERA)) {
                new_nodeA->size = previous->size;
                previous->size = 0;
                memset(curr_memory + sizeof(HEADER) + sizeof(HEADERA), -1, new_nodeA->size);
                return (curr_memory + sizeof(HEADER) + sizeof(HEADERA));
            }
            //tu sa dá pama ešte využi a preto tu vytváram ïalšiu hlavièku, ktorá bude ukazova na novovzniknutú vo¾nú èas pamata
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
        //cyklus, korý slúži na to, aby som našiel predchádzajúci blok pamäta a mohol ho spoji s novým 
        while (i != pozicia - 1) {
            previous = previous->next;
            i++;
        }

        //nejako si potrebujem zabezbeèi predchodcu pointer curr, pretože budem curr vymazáva, pretože nebude odkazova 
        //vo¾ne miesto v pamäti, tak tým pádom mi je tento pointer zbytoèný a môžem ho použi skôr na alokáciu pamäte

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

    //ak sa do funkcie free pošle NULL pointer

    char* header = (char*)pointer;
    HEADER* hlavicka = (HEADER*)pointer;
    //toto je prípad, keï uvolòujem takú pama, ktorá v¾avo susedí s hlavnou hlavièkou, ktorá ukazuje na zaèiatok celého po¾a
    //môžu nasta 3 prípady:

    if (curr_memory == (header + sizeof(HEADER))) {

        //toto je prípad ak je v pamati alokovana jedna ve¾ká pama, ktorá ide až po koniec celeho pola
        if ((curr_memory + sizeof(HEADERA) + sizeOfMemory) == (header + hlavicka->sizeOfMemory)) {
            hlavicka->size += sizeOfMemory;
            memset(curr_memory, -2, sizeOfMemory + sizeof(HEADERA));
            return 0;
        }

        //druhý prípad je, že vpravo od nej je pama, ktorá je alokovaná => zvýšim ve¾kos vo¾nej pamate v hlavnej hlavièke
        else if (*(curr_memory + sizeOfMemory + 2 * sizeof(HEADERA)) == -1) {
            hlavicka->size += sizeOfMemory;
            memset(curr_memory, -2, sizeOfMemory + sizeof(HEADERA));
            return 0;
        }

        //tretí prípad je, že vpravo od nej sa nachádza pama, ktorá je taktiež vo¾ná => spojím všetky bloky pamate
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
    //prípad ak pred danou pamäou je taktiež vo¾ná pamä 
    else if (*(curr_memory - 1) == -2) {
        //prípad ak pred danou pamaou je vo¾ná pama a zároven za danou pamaou je hlavièka, ktorá ukazuje na ïalšiu vo¾nú pama
        //robím to preto, aby som vybral taký blok pamate, s ktorým spojim danu pama, aby výsledná pama bola èo najvaèšia


        //ak je toto if pravdivé znamená to že daná pama, ktoru chcem uvolni je ohranièená dvoma dalsimi volnymi pamatami -> ze spojim tieto 3 bloky pamate
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
        //toto je prípad, ak je pred pamäou vo¾ná pama a za pamaou nie -> takže spojím tieto dve vo¾né pamate do jednej väèšej
        else {
            int index = 1;
            while (*(curr_memory - index) == -2) {
                index++;
            }
            index--;

            //budem h¾ada index tej hlavièky ktorá je najbližšia tejto vo¾nwj pamate

            HEADER* new_header = (HEADER*)(curr_memory - sizeof(HEADER) - index);
            new_header->size += sizeOfMemory + sizeof(HEADERA);

            curr_memory = (char*)valid_ptr;
            memset(curr_memory - sizeof(HEADERA), -2, sizeOfMemory + sizeof(HEADERA));

            return 1;
        }
    }
    //prípad, ktorý hovorí to, že uvolnujem takú pamat, ktorá je na konci celeho pola a pred sebou má alokovanu pamat
    else if (curr_memory + sizeof(HEADERA) + helpMemory->size == header + hlavicka->sizeOfMemory) {
        int index = 1;

        //cyklus mi h¾adá najbližiu hlavièku vo¾nej pamate, tam nie je tak nájde hlavièku, ktorá ukazuje na zaèiatok celeho pola
        while (1) {
            if (*(curr_memory - index) != -1) {
                if (*(curr_memory - index - sizeof(HEADERA)) == -1) { //podmienky pomocou ktorých kontrolujem èi sa na danej adrese nenachadza alokovana èas
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
        //nájdenie predchádzajúcej hlavièky, aby som ju mohol spoji s novou
        HEADER* previous = (HEADER*)(curr_memory - index - sizeof(HEADER));
        int size = helpMemory->size;
        //nová hlavièka
        HEADER* new_header = (HEADER*)(curr_memory);
        new_header->size = size;
        new_header->sizeOfMemory = previous->sizeOfMemory;
        new_header->next = previous->next;
        previous->next = new_header;

        memset(curr_memory + sizeof(HEADER), -2, size + sizeof(HEADERA) - sizeof(HEADER));

        return 1;
    }
    //prípad ak pred danou pamaou nie je vo¾ná pama ale alokovaná a za danou pamäou sa nachádza hlavièka vo¾ného bloku
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
    //prípad keï pred pamatou je alokovaná aj za pamaou je alokoivana pama
    //OKOMENTOVAT DONE!!!
    else {
        int index = 1;

        //cyklus mi h¾adá najbližiu hlavièku vo¾nej pamate, tam nie je tak nájde hlavièku, ktorá ukazuje na zaèiatok celeho pola
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

        //nájdenie predchádzajúcej hlavièky, aby som ju mohol spoji s novou
        HEADER* previous = (HEADER*)(curr_memory - index - sizeof(HEADER));
        int size = helpMemory->size;
        //nová hlavièka
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

//funkcia, ktora služi na spoèítanie alokovanej èasti pamäte
void memory_count(void* valid_ptr, int size, int sizeOfAlloc) {
    char* start = (char*)valid_ptr + sizeof(HEADER);
    HEADERA* sizeOfAllocated = (HEADERA*)start;
    int count = sizeof(HEADER);
    int countOfHeaders = 0;
    int curr_memory = sizeof(HEADER);
    int temp;
    printf("\tAlokacia velkosti %dB pri velkosti pamate %dB.\n\n", sizeOfAlloc, size);
    //cyklus bude prebiaha dovtedy, kým sa neprejde celé pole
    while (count != size) {
        //cyklus može zasta ak sa na danej adrese nenachádza -1, èo znamená, že sa tam nachádza bude vo¾na pama alebo nejaká hlavièka vo¾neho bloku
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

//zhodná funkcia ako memory_count1, len je to vytvorene pre testy, kde sa hodnoty udávajú
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

//testovaè pre test 1
//odskúšam tu všetky funkcie: memory_alloc, memory_free, memory_check aj memory_init
void test1(int size, void* arrayOfPointers[], int sizeOfAlloc) {
    int index = 0;
    memory_init(pointer, size);
    //iterujem až dovtedy, kým mi funkcia malloc nevráti NULL èo znamená, že už tam nie je miesto na alokáciu danej veškosti pamäte
    while (1) {
        arrayOfPointers[index] = memory_alloc(sizeOfAlloc);
        if (arrayOfPointers[index] == NULL) {
            break;
        }
        index++;
    }
    //následne sa zráta % alokovanej pamäte
    memory_count(pointer, size, sizeOfAlloc);
    //uvo¾nia sa jednotlive bloky pamäte
    for (int i = 0; i < index; i++) {
        memory_free(arrayOfPointers[i]);
    }

}
//testovaè na testy 2,3,4
//podobná funkcia ako na test 1, len s tým rozdielom, že sa vo funkcií generujú náhodne ve¾kosti pamäte, ktoré budem alokova
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
    //generovanie náhodných ve¾kostí pamäte mám vyriešené tak, že
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
