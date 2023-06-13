/**
 * @file main.c 
 * @author adaskin
 * @brief this simulates the movement of a hunter and animal game in 2D site grid
 * @version 0.1
 * @date 2023-05-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <stdio.h>
#include <stdlib.h>


typedef enum { BEAR, BIRD, PANDA} AnimalType;

typedef enum { ALIVE, DEAD } AnimalStatus;

typedef struct {
    int x;
    int y;
} Location;

typedef enum { FEEDING, NESTING, WINTERING } SiteType;

typedef struct {
    /** animal can be DEAD or ALIVE*/
    AnimalStatus status;
    /** animal type, bear, bird, panda*/
    AnimalType type;
    /** its location in 2D site grid*/
    Location location;
} Animal;

/*example usage*/
Animal bird, bear, panda;

/** type of Hunter*/
typedef struct {
    /** points indicate the number of animals, a hunter killed*/
    int points;
    /** its location in the site grid*/
    Location location;
} Hunter;

/** type of a site (a cell in the grid)*/
typedef struct {
    /** array of pointers to the hunters located at this site*/
    Hunter **hunters;
    /** the number of hunters at this site*/
    int nhunters;
    /** array of pointers to the animals located at this site*/
    Animal **animals;
    /** the number of animals at this site*/
    int nanimals;
    /** the type of site*/
    SiteType type;
} Site;

/** 2D site grid*/
typedef struct {
    /** number of rows, length at the x-coordinate*/
    int xlength;
    /** number of columns, length at the y-coordinate*/
    int ylength;
    /** the 2d site array*/
    Site **sites;
} Grid;

/* initial grid, empty*/
Grid grid = {0, 0, NULL};

/**
 * @brief initialize grid with random site types
 * @param xlength
 * @param ylength
 * @return Grid
 */
Grid initgrid(int xlength, int ylength) {
    grid.xlength = xlength;
    grid.ylength = ylength;

    grid.sites = (Site **)malloc(sizeof(Site *) * xlength);
    for (int i = 0; i < xlength; i++) {
        grid.sites[i] = (Site *)malloc(sizeof(Site) * ylength);
        for (int j = 0; j < ylength; j++) {
            grid.sites[i][j].animals = NULL;
            grid.sites[i][j].hunters = NULL;
            grid.sites[i][j].nhunters = 0;
            grid.sites[i][j].nanimals = 0;
            double r = rand() / (double)RAND_MAX;
            SiteType st;
            if (r < 0.33)
                st = WINTERING;
            else if (r < 0.66)
                st = FEEDING;
            else
                st = NESTING;
            grid.sites[i][j].type = st;
        }
    }

    return grid;
}

/**
 * @brief
 *
 */
void deletegrid() {
    for (int i = 0; i < grid.xlength; i++) {
        free(grid.sites[i]);
    }

    free(grid.sites);

    grid.sites = NULL;
    grid.xlength = -1;
    grid.ylength = -1;
}

/**
 * @brief prints the number animals and hunters in each site
 * of a given grid
 * @param grid
 */
void printgrid() {
    for (int i = 0; i < grid.xlength; i++) {
        for (int j = 0; j < grid.ylength; j++) {
            Site *site = &grid.sites[i][j];
            int count[3] = {0}; /* do not forget to initialize*/
            for (int a = 0; a < site->nanimals; a++) {
                Animal *animal = site->animals[a];
                count[animal->type]++;
            }

            printf("|%d-{%d, %d, %d}{%d}|", site->type, count[0], count[1],
                   count[2], site->nhunters);
        }
        printf("\n");
    }
}

/**
 * @brief prints the info of a given site
 *
 */
void printsite(Site *site) {
    int count[3] = {0}; /* do not forget to initialize*/
    for (int a = 0; a < site->nanimals; a++) {
        Animal *animal = site->animals[a];
        count[animal->type]++;
    }
    printf("|%d-{%d,%d,%d}{%d}|", site->type, count[0], count[1], count[2],
           site->nhunters);
}

/*
============================================================= 
TODO: you need to complete following three functions 
DO NOT CHANGE ANY OF THE FUNCTION NAME OR TYPES
============================================================= 
*/
/*include library*/
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define TIMEOUT_SECONDS 1

/* Error flag is for move_animal_to_site. simulate_animal fun uses.*/
int exit_flag = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
/**
 * @brief  it moves a given hunter or animal
 * randomly in the grid
 * @param args is an animalarray
 * @return void*
 */
/* This function is for hunter loop controlling. */ 
int is_there_animal() { 
	for (int i = 0; i < grid.xlength; i++) {
        for (int j = 0; j < grid.ylength; j++) {
            Site *site = &grid.sites[i][j];
			if (site->nanimals>0) return 1;
		}
	}
	/* There is no animal in the jungle */ 
	return 0;
} 
/* This function is for nesting and starting animal. */ 
void add_animal_to_site(Site *site, Animal *newAnimal){
	int array_size = sizeof(*site->animals) / sizeof(site->animals[0]); /* Define array size */
	if (array_size<5){
        site->animals = (Animal**)realloc(site->animals, sizeof(Animal*) * (5)); /* Reallocation */
		/* CONTROL */
		if (site->animals == NULL){
			perror("Realloc ERROR:\t");
			exit(1);
		}
	}		
	/* Update site */
	site->animals[site->nanimals] = newAnimal;
    site->nanimals++;
}
/* This function updates location values. Parameters are old location(x,y). */
int randomlocation(int x,int y){
	/* Random generator */
	int r = rand() % 4;
    /* location variable control */
    if (r == 0 && x > 0) x--;
    else if (r == 1 && y < grid.xlength - 1) y++;
	else if (r == 2 && y > 0) y--;
    else if (r == 3 && y < grid.ylength - 1) y++;
    /* return newx and newy */
    return x,y;
} 
/* This function updates site values. Parameters are old and new site, animal and new location. */
int move_animal_to_site(Site *site, Site *newsite, Animal *animal, int newx, int newy) {
	int array_size = sizeof(*newsite->animals) / sizeof(newsite->animals[0]); /* Define array size */
    int index = -1; /* Index flag for swap*/

    Animal *tmp = NULL; /* Temprory animal */

    for (int i = 0; i < site->nanimals; i++) {
        if (animal == site->animals[i]) { /* Find the animal in animals */
            index = i; /* index update */
            break;
        }
    }
    if (index >= 0) { /* Is animal in the animals */
        if (index != site->nanimals - 1) {
            tmp = site->animals[site->nanimals - 1]; /* Swap */
            site->animals[site->nanimals - 1] = site->animals[index]; /* Swap */
            site->animals[index] = tmp; /* Swap */
        }
    } else {  /* animal didn't find in animals*/
        printf("ERROR \t %d\n",index);
        return 0;
    }
    if (array_size<5){
        newsite->animals = (Animal**)realloc(newsite->animals, sizeof(Animal*) * (5)); /* Reallocation */
        /* Error control */
        if (site->animals == NULL){
			perror("Realloc ERROR:\t");
			return 0;
		}
	}
    /* Animal location update */
    animal->location.x = newx;
    animal->location.y = newy;
	/* Site information update */
    site->nanimals--; 
    site->animals[site->nanimals] = NULL;
    newsite->animals[newsite->nanimals] = animal;
    newsite->nanimals++;
    return 1;
}

void *simulateanimal(void *args) {
	Animal *animal = (Animal *)args; /*take and define argument*/
	int x,y,newx,newy; /*location variable*/
	double random_value=0.0; /*random number for winter and feeding*/
	
	double diff=0.0; /* Diff t1 t2 */
	
	time_t t1,t2; /* Timeout */
	t1 = time(NULL); /* Starting time */
	
	/* location is assigned to variables */
	x = animal->location.x;
    y = animal->location.y;
    newx = x;
    newy = y;
	
	while(animal->status != DEAD){
		
		/* HUNTING */
		if(grid.sites[animal->location.x][animal->location.y].nhunters>0){
			grid.sites[x][y].nanimals--;
			grid.sites[x][y].animals[grid.sites[x][y].nanimals] = NULL;
			animal->status = DEAD;
			printf("Animal is hunting\n"); /* info TEST*/
			break;
		}
		/* SITE TYPE SWITCH */
		switch (grid.sites[animal->location.x][animal->location.y].type){
			case NESTING:
				printf("Animal is nesting\n"); /* info TEST*/
				newx,newy = randomlocation(x,y);
				break;
				
			case WINTERING:
				printf("WINTER\n");
				random_value = (double)rand() / RAND_MAX; /* Random generator is for decision */
				
				/* Die with 0.5 probability */
				if(random_value <= 0.5){ 
					grid.sites[x][y].nanimals--;
					grid.sites[x][y].animals[grid.sites[x][y].nanimals] = NULL;
					animal->status = DEAD;
					printf("Animal is dying\n"); /* info TEST*/
					break;
				}
				/*Live and move with 0.5 probability*/
				else
					newx,newy = randomlocation(x,y);	
                break;
                
            case FEEDING:
				random_value = (double)rand() / RAND_MAX; /* Random generator */
				/* All stay there with 0.8 probability */
				if(random_value <= 0.8){
					printf("Animal is feeding\n"); /* info TEST*/
				}
				/* Move with 0.5 probability */
				else
					newx,newy = randomlocation(x,y);
				break;
		}
		/* If animal dosen't move */
		if(x==newx && y==newy){
			usleep(1000); /* Animal slept */
			
			/* TIME CONTROL */
			t2 = time(NULL);
			diff = difftime(t2, t1); 
			if(diff>=1.0){
				printf("TIMEOUT: %.2f second\n", diff); /* info TEST*/
				break;
			}
			continue;
		}
		/* Update nanimals and animals */
		exit_flag = move_animal_to_site(&grid.sites[x][y],&grid.sites[newx][newy],animal,newx,newy);
		/* ERROR CONTROL */
		if(exit_flag==0 || exit_flag==-1) break;
		
		/* Update x and y */
		x = newx;
		y = newy;
		
		usleep(1000); /* Animal slept */
		/* TIME CONTROL */
		t2 = time(NULL);
		diff = difftime(t2, t1);
		if(diff>=1.0){
			printf("TIMEOUT: %.2f second\n", diff);
			break;
		}
	}
	/* Exit the thread */
    pthread_exit(NULL);
}

/**
 * @brief simulates the moving of a hunter
 *
 * @param args
 * @return void*
 */
void *simulatehunter(void *args) {
	/*Take and define argument*/
    Hunter *hunter = (Hunter *)args;
	int x,y,newx,newy; /*x and y are location variables newx and newy for new location.*/
	
	time_t t1, t2; /* Time variables define */
    double diff = 0.0; /* Diff t1 t2 */
    t1 = time(NULL); /* Starting time */
	
	/* Initialize x, y, newx and newy */
	x = hunter->location.x;
	y = hunter->location.y;
	newx = x;
	newy = y;
	
	grid.sites[x][y].nhunters++; /* Site updated */
	
	usleep(1000); /*Hunter slept.*/

	while(is_there_animal()){
		newx,newy =randomlocation(x,y); /* New location varibles updated */
		
		/* If hunter dosen't move */
		if(x==newx && y==newy){ 
			usleep(1000);
			
			/* TIME CONTROL */
			t2 = time(NULL);
			diff = difftime(t2, t1);
			if(diff>=1.0){
				printf("TIMEOUT: %.2f second\n", diff);
				break;
			}
			continue;
		}
		
		/* Site variables update */
		grid.sites[hunter->location.x][hunter->location.y].nhunters--;
		
		hunter->location.x = newx;
		hunter->location.y = newy;
		
		grid.sites[hunter->location.x][hunter->location.y].nhunters++;
		
		/* x and y updating */
		x = newx;
		y = newy;
		
		usleep(1000);
		/* TIME CONTROL */
		t2 = time(NULL);
		diff = difftime(t2, t1);
		if(diff>=1.0){
			printf("TIMEOUT: %.2f second\n", diff);
			break;
		}
	}
	/* Exit the thread */
	pthread_exit(NULL);
}

/**
 * the main function for the simulation
 */
int main(int argc, char *argv[]) {
	
    initgrid(3, 3); /* initialize grid */

	int err; /*thread control variable*/ 
	
	/*input control*/
	if (argc != 2) {
        printf("please enter a number!\n");    
        return 1;
    }
    /*input control*/
    if (atoi(argv[1]) < 0){
        printf("Number of hunters must be non-negative!\n");
        return 1;
	}
    int nHunters = atoi(argv[1]); /*define length of an array*/ 
	Hunter *hunters1 = (Hunter *)malloc(sizeof(Hunter) * nHunters);	/*define hunters array*/ 
	
	/* Malloc error control*/ 
	if(hunters1 == NULL){
		perror("Malloc ERROR:\t");
		exit(1);
	}
	
	/*define threads*/ 
	pthread_t hunter_threads[nHunters];
    pthread_t bird_thread, bear_thread, panda_thread;
    
    srand(time(NULL));/*for random generator*/
	
	/* INIT ANIMALS */
	
	/*initialize bird */
	bird.location.x = rand() % grid.xlength;
    bird.location.y = rand() % grid.ylength;
    bird.type = BIRD;
    bird.status = ALIVE;
	add_animal_to_site(&grid.sites[bird.location.x][bird.location.y],&bird); /* adding bird to animals. */
	
	/*initialize bear */
	bear.location.x = rand() % grid.xlength;
    bear.location.y = rand() % grid.ylength;
    bear.type = BEAR;
    bear.status = ALIVE;
	add_animal_to_site(&grid.sites[bear.location.x][bear.location.y],&bear); /* adding bear to animals. */
	
	/*initialize panda */
	panda.location.x = rand() % grid.xlength;
    panda.location.y = rand() % grid.ylength;
    panda.type = PANDA;
    panda.status = ALIVE;
	add_animal_to_site(&grid.sites[panda.location.x][panda.location.y],&panda); /* adding panda to animals. */
    
    /* INIT HUNTERS */
    for (int i = 0; i < nHunters; i++) {
        hunters1[i].points = 0;
        hunters1[i].location.x = rand() % grid.xlength;
        hunters1[i].location.y = rand() % grid.ylength;
	}
	
	/*printgrid();*/   /* TEST */
	/*printf("\n");*/  /* TEST */
	
	/*hunters threads are created*/
	for(int i=0;i<nHunters;i++)
			err = pthread_create(&hunter_threads[i], NULL, simulatehunter, (void *)&hunters1[i]);
			if (err != 0){ 
				perror("Hunter Thread ERROR:\t");  /*Thread error control*/
				exit(1);
			}
	/*create animals threads*/
	err = pthread_create(&bird_thread, NULL, simulateanimal, (void *)&bird);
	
	if (err != 0){ 
		perror("Animal Thread ERROR:\t");       /*Thread error control*/
		exit(1);
	}
	err = pthread_create(&bear_thread, NULL, simulateanimal, (void *)&bear);
	
	if (err != 0){ 
		perror("Animal Thread ERROR:\t"); 		/*Thread error control*/
		exit(1);
	}
	err = pthread_create(&panda_thread, NULL, simulateanimal, (void *)&panda);
	
	if (err != 0){ 
		perror("Animal Thread ERROR:\t");		/*Thread error control*/
		exit(1);
	}
	/*hunters threads are waiting*/
	for(int i=0;i<nHunters;i++)
		pthread_join(hunter_threads[i], NULL);
	
	/*animals threads are waiting*/
	pthread_join(bird_thread, NULL);
	pthread_join(bear_thread, NULL);
	pthread_join(panda_thread, NULL);
    
    
    printgrid();
    deletegrid();
    free(hunters1);
    return 0;
}

