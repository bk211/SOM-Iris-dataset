#define _GNU_SOURCE
#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <assert.h>
const int vec_size = 150;
size_t buffer_size = 27;

const int vec_att_size = 4;
const int neu_size = 60;
const double random_weight_max = 1.025;
const double random_weight_min = 0.95;
/*-0.05 +0.025*/
const int neu_sizeX = 10;
const int neu_sizeY = 6;
const int learning_prop_radius = 3;
const double learning_alpha = 0.9;
const int nb_learning_cycle = 500;
const int refine_prop_radius = 1;
const double refine_alpha = 0.09;
const int nb_refine_cycle = 1500;

typedef struct flower flower_t;
struct flower
{
    int type;
    double * data;
};

typedef flower_t ** network;

/*liste chainée
*/
typedef struct node node_t;
typedef struct node * list;
struct node {
  int x,y;
  double diff;
  list next;
};

/*tete de liste
*/
typedef struct head head_t;
struct head{
    double best_diff;
    int counter;
    list next;
};


/*affiche une fleur
*/
void print_fleur(flower_t f){
    printf("%f %f %f %f %d\n", f.data[0],f.data[1],f.data[2],f.data[3], f.type);
}


/* compare 2 chaine de charactere, re-implementation de strcmp
retourne 1 si identique 0 sinon
*/
int my_str_cmp(char * src, char * src2, int size_of_src2){
    int i;
    for (i = 0; i < size_of_src2; i++){
        if(src[i] != src2[i])
            return 0;
    }
    return 1;
}


/*remplie le vecteur à l'indice avec les données
*/
void fill_vector(int indice, flower_t * vec, int datasize, double * data, char * name){
    vec[indice].data = (double*)malloc(vec_att_size * sizeof(double));
    int i;
    for (i = 0; i < datasize; i++){
        vec[indice].data[i] = data[i];
    }
    
    if(my_str_cmp(name, "Iris-setosa",11)){
        vec[indice].type = 1;
    }else if(my_str_cmp(name, "Iris-versicolor",15)){
        vec[indice].type = 2;
    }else if(my_str_cmp(name, "Iris-virginica",14)){
        vec[indice].type = 3;
    }
    else{
        printf("<%s>\n",name);
        fprintf(stderr,"erreur de reconnaissance de fleur\n");
        exit(1);
    }

    /*ancienne version avec strcomp, visiblement il y a un pb l'identification des chaines*/
    /* 
    if(strcmp(name, "Iris-setosa\n") == 0){
        vec[indice].type = 1;
    }else if(strcmp(name, "Iris-versicolor\n") == 0){
        vec[indice].type = 2;
    }else if(strcmp(name, "Iris-virginica\n") == 0){
        vec[indice].type = 3;
    }else{
        printf("<%s>\n",name);
        fprintf(stderr,"unknow type\n");
        exit(1);
    }*/
}

/*moyenne d'une colonne
*/
double get_average(flower_t * vec, int col_number, int size){
    double ret = 0;
    int i;
    for (i = 0; i < size; i++){
        /*printf("%f ,",vec[i].data[col_number]);*/
        ret += vec[i].data[col_number];
    }
    ret /= size;
    return ret;
}

/*norme d'une fleur 
*/
double get_norme(flower_t vec, int att_size){
    double ret = 0;
    int i;
    for (i = 0; i < att_size; i++){
        ret += vec.data[i] * vec.data[i];
    }
    ret = sqrt(ret);
    return ret;
}

/*normalise un vecteur vec, le resultat normalisé est dans ret
*/
void normalize(flower_t * vec, int size, int att_size, flower_t * ret){
    int i,j;
    for (i = 0; i < size; i++){
        ret[i].data = malloc(att_size * sizeof(double));
        for (j = 0; j < att_size; j++){
            ret[i].data[j] = vec[i].data[j]/ get_norme(vec[i], att_size);
        }
        ret[i].type = vec[i].type;
    }
}

/*retourne un coeff aléatoire selon le max et min
*/
double get_random_weight(){
    double result = random_weight_min + ((double) rand() / (double) RAND_MAX) * (double)(random_weight_max - random_weight_min);
    /*printf("%f ", result);*/
    return result;
}

/*remplie aléatoirement un neurone
*/
void fill_neuron(flower_t * neu, flower_t av_neu, int att_size){
    neu->data = (double*) malloc(att_size * sizeof(double));
    int i;
    for (i = 0; i < att_size; i++){
        neu->data[i] = av_neu.data[i] * get_random_weight();
        /*neu->data[i] = av_neu.data[i];*/
        neu->type = 0;
    }
}
/*affiche le reseau
*/
void print_network(network r){
    int i,j;
    for (i = 0; i < 6; i++){
        for (j = 0; j < 10; j++){
            printf("%d : ",i*10+ j);
            print_fleur(r[i][j]);
        }
    }
}

/*crée un reseau de neuronne
*/
network create_neurons(flower_t vec_av, int neu_size, int att_size, int sizeX, int sizeY){

    network result = (flower_t **) malloc( sizeY * sizeof(flower_t*));
    assert(result);
    int i,j;
    for (i = 0; i < sizeX; i++){
        result[i] = (flower_t *) malloc(10 * sizeof(flower_t));
    }

    /*printf("%d \n", neu_size);
    printf("%d \n", neu_size / 10);
    */
    
    for (i = 0; i < sizeY ; i++){    
        for (j = 0; j < sizeX; j++){
            fill_neuron(&result[i][j], vec_av, att_size);
        }
    }

    return result;
        
}

/*compare 2 fleur en fct de la distance euclidienne
*/
double compare_neuronne(flower_t f1, flower_t f2, int att_size){
    double result = 0;
    int i;
    for ( i = 0; i < att_size; i++){
        result += sqrt( (f2.data[i] - f1.data[i]) * (f2.data[i] - f1.data[i]) );
        /*printf("res =%f\n",result);*/
    }
    return result;
}

/*remplie le tableau d'indice aléatoirement
*/
void fill_random_index_arr(int * indice,int size){
    int i;
    for ( i = 0; i < size; i++){
        indice[i] = i;
    }

    int draw, tmp;
    for (i = 0; i < size; i++){
        draw = rand() % size;
        tmp = indice[i];
        indice[i] = indice[draw];
        indice[draw] = tmp;
    }
}

/*crée un noeud 
*/
list create_node(double diff, int y, int x){
    list l;
    l = (list) malloc(sizeof(node_t));
    l->diff = diff;
    l->y = y;
    l->x = x;
    l->next = NULL;
    /*printf("CN :x = %d y= %d diff= %f\n", l->x, l->y, l->diff);*/
    return l;
}
/*retourne le noeud à l'indice index
*/
list get_node(list h, int index){
    list result = h;
    assert(index >=0);
    while (index != 0){
        result = result->next;
        index--;
    }
    return result;
}
/*affiche la liste 
*/
void print_list(list l){
    if(l != NULL){
        printf("x = %d y= %d diff= %f\n", l->x, l->y, l->diff);
        /*printf("x = %d y= %d diff= %f\n", l->next->x, l->next->y, l->next->diff);*/
        print_list(l->next);
    }
}

/*crée un nouveau noeud et le met à la tête de liste
*/
void push_front(head_t * head, double diff, int y, int x){
    
    
    list new_l = create_node(diff, y, x);
    list tmp = NULL;
    
    if(head->next == NULL){
        head->best_diff = diff;
        head->next = new_l;
        head->counter = 1;
        /*printf("PFN = %d y= %d diff= %lf\n", head->next->x, head->next->y, head->next->diff);
        print_list(head->next);*/
    }else{
        /*printf("in else:\n");*/
        head->best_diff = diff;
        tmp = head->next;
        head->next = new_l;
        new_l->next = tmp;

        if(diff < tmp->diff){
            /*printf("better diff\n");*/
            head->counter = 1;
        }else{
            /*printf("equal diff\n");*/
            head->counter ++;
        }
    }  
    
}

/*choisit un best match dans la liste chainée
*/
void select_best_match(int * x, int *y, head_t *h){
    /*print_list(h->next);*/
    int choice = rand() % h->counter;
    list chosen_node = get_node(h->next, choice);
    *x = chosen_node->x;
    *y = chosen_node->y;
    /*printf("[%d %d] [%d %d]\n", *x, *y, chosen_node->x, chosen_node->y);*/
}

/* libère la mémoire de la liste chainée
*/
void free_list(list node){
    if(node->next)
        free_list(node->next);
    free(node);
}

/*trouve le best match, ses coordonnées sont affecté à x et y
*/
void find_best_match(int * x, int *y, network neu, flower_t data, int att_size,int sizeX, int sizeY){
    head_t head_list;
    head_list.best_diff = 999.0;
    head_list.counter = 0;
    head_list.next =NULL;
    double tmp_diff;
    int i,j;
    for (i = 0; i < sizeY; i++){
        for (j = 0; j < sizeX; j++){
            tmp_diff = compare_neuronne(data, neu[i][j], att_size);
            if(tmp_diff <= head_list.best_diff){ /* if better matching*/
                /*printf("FB : xy = %ld %ld tmp diff = %lf\n", j ,i, tmp_diff);*/
                push_front(&head_list, tmp_diff, i, j);
    /*            printf(">x = %d y= %d diff= %lf\n", head_list.next->x, head_list.next->y, head_list.next->diff);*/
            }
        }    
    }

    /*printf("c = %d, best =%lf\n",head_list.counter, head_list.best_diff);*/
    select_best_match(x, y, &head_list);
    /*print_list(head_list.next);*/
    free_list(head_list.next);
}

/*propage l'information gagnat parmi le voisinage 
*/
void propagate(network neu, int att_size, flower_t learning_data, int * neighbours, int size, double alpha){
    int i,j;
    for ( i = 0; i < size; i+=2){ /* for each neighbours*/
        
        for ( j = 0; j < att_size; j++){
            /*printf("%d %d");*/
            neu[neighbours[i+1]][neighbours[i]].data[j] = neu[neighbours[i+1]][neighbours[i]].data[j] + alpha * (learning_data.data[j] - neu[neighbours[i+1]][neighbours[i]].data[j]); 
        }
        
    }
    
}

/*retourne un tableau de coordonée xy qui contient tous les voisins
*/
int find_neighbours(network neu, int neu_size, int sizeX, int sizeY, int winX, int winY, int * storage, int radius){
    int size = 0;
    int beginX = ((winX - radius) > 0) ? (winX - radius) : 0;
    int endX = ((winX + radius) < sizeX) ? (winX + radius+1) : sizeX;
    int beginY = ((winY - radius) > 0) ? (winY - radius) : 0;
    int endY = ((winY + radius) < sizeY) ? (winY + radius+1) : sizeY;
    int i,j;
    for (i = beginY; i < endY; i++){
        for (j = beginX; j < endX; j++){
            /*printf("[X=%ld Y=%ld] c = %d\n", j,i, size);*/
            storage[size++] = j;
            storage[size++] = i;
        }
    }

    return size;
}

/*cycle d'apprentissage
*/
void learning_cycle(network neurons, int size, int sizeX, int sizeY, int att_size,flower_t * learning_vec, int learning_vec_size, int * index, int index_size, int * neighbours, double prop_alpha, int neighbours_radius){
    int x, y;
    fill_random_index_arr(index, index_size);
    int i;
    int neighbours_size = 0;
    for (i = 0; i < learning_vec_size; i++){/*iterate over 150 index*/
        find_best_match(&x, &y, neurons, learning_vec[index[i]], att_size, sizeX, sizeY);
        /*printf("Learning data : ");
        print_fleur(neurons[y][x]);
        print_fleur(learning_vec[index[i]]);
        printf("FBM end: x=%d y=%d \n", x,y);
        */
        neighbours_size = find_neighbours(neurons, size, sizeX, sizeY, x, y, neighbours, neighbours_radius);
        /*printf("nb =%d\n",neighbours_size);*/
        propagate(neurons, att_size , learning_vec[index[i]], neighbours, neighbours_size, prop_alpha);
        /*print_fleur(neurons[y][x]);*/
    }    
}

/*determine la nature du neurone 
*/
void mark_neurons(network neurons, int sizeX, int sizeY,flower_t * learning_vec,int vec_size, int att_size, int * index){
    int x, y;
    int i;
    for (i = 0; i < vec_size; i++){
        find_best_match(&x, &y, neurons, learning_vec[index[i]], att_size, sizeX, sizeY);
        if(neurons[y][x].type == 0){
            neurons[y][x].type = learning_vec[index[i]].type;
        }
    }
}

/*affiche le résultat du reseau de neuronne 
*/
void show_result(network neurons, int sizeX, int sizeY){
    int tab[4] = {0, 0,0,0};
    int i,j;
    for (i = 0; i < sizeY; i++){
        for (j = 0; j < sizeX; j++){
            printf("%d ", neurons[i][j].type);
            tab[neurons[i][j].type]++;
        }
        printf("\n");
    }

    printf("0=%d\t1=%d\t2=%d\t3=%d\n", tab[0], tab[1], tab[2], tab[3]);
}


int main(int argc, char const *argv[]){
    if(argc != 2){
        printf("Usage: ./exec file_name\n");
        exit(1);
    }

    srand(time(NULL));

    const char * fname = argv[1];
    /*printf("filename: %s\n", fname);*/
    
    FILE * file = fopen(fname,"r");
    flower_t * vec_data = (flower_t *)malloc(vec_size * sizeof(flower_t));
    flower_t * normalized_vec_data = (flower_t *)malloc(vec_size*sizeof(flower_t));
    
    char * line = (char *) malloc(buffer_size* sizeof(char));
    double * buffer_data = (double *) malloc(vec_att_size * sizeof(double));
    char* buffer_type;
    char * buffer_reader;
    int i,j;
    for (i = 0; i < vec_size; i++){
        getline(&line, &buffer_size, file);
        /*printf("%s",line);*/
        
        buffer_reader = strtok(line, ",");
        for (j = 0; j < 4; j++)
        {
            buffer_data[j] = strtod( buffer_reader,NULL);
            buffer_reader = strtok(NULL, ",");
        }
        /*printf("%lf %lf %lf %lf ", buffer_data[0], buffer_data[1], buffer_data[2], buffer_data[3]);*/
        buffer_type = buffer_reader;
        /*printf(">> %d\n", sizeof(buffer_type));*/
        /*printf("%s", buffer_type);*/
        fill_vector(i, vec_data, vec_att_size, buffer_data, buffer_type);

    }

    fclose(file);

    /*for (size_t i = 0; i < 150; i++){print_fleur(vec_data[i]);}*/
    
    /*normalisation
    */
    normalize(vec_data, vec_size, vec_att_size, normalized_vec_data);
    /*for (size_t i = 0; i < 150; i++){print_fleur(normalized_vec_data[i]);}*/

    /*vecteur contenant les valeurs moyennes*/
    flower_t vec_average;
    vec_average.type = 0;
    vec_average.data = malloc(vec_size * sizeof(double));
    
    for (i = 0; i < vec_att_size; i++){
        vec_average.data[i] = get_average(normalized_vec_data, i, vec_size);
        /*printf("%f ,", vec_average.data[i]);*/
    }

    /*print_fleur(vec_average);*/
    
    /*crée et initialise le réseau de neurone avec le vecteur moyen
    */
    network neurons = create_neurons(vec_average, neu_size, vec_att_size , neu_sizeX, neu_sizeY);
    assert(neurons);
    
    /*print_network(neurons);*/
    
    /*genese tableau randomisation*/
    int *index = (int*)malloc(vec_size * sizeof(int));
    assert(index);
    fill_random_index_arr(index,vec_size);
    /*for (size_t i = 0; i < 150; i++){printf("%d ", index[i]);}*/
    
    /*printf("=> %f \n", compare_neuronne(neurons[0][0], neurons[0][1], vec_att_size));
    */
    
    /*allocation mémoire pour le tableau de voisins
    */
    int * neighbours = (int*) malloc(2 * (2*learning_prop_radius+1) * (2*learning_prop_radius+1) * sizeof(int));
    assert(neighbours);

    /*cycle d'apprentissage
    */
    double learning_step = learning_alpha / nb_learning_cycle;
    double k;
    for (k = learning_alpha; k > 0; k -= learning_step){
        learning_cycle(neurons, neu_size, neu_sizeX, neu_sizeY, vec_att_size , normalized_vec_data, vec_size, index, vec_size, neighbours, k, learning_prop_radius);
    }
    
    /*cycle de rafinage
    */
    double refine_step = refine_alpha / nb_refine_cycle;
    for ( k = refine_alpha; k > 0; k -= refine_step){
        learning_cycle(neurons, neu_size, neu_sizeX, neu_sizeY, vec_att_size , normalized_vec_data, vec_size, index, vec_size, neighbours, k, refine_prop_radius);
    }
    
    /*melange une dernière fois le tableau d'indice
    */
    fill_random_index_arr(index,vec_size);

    /*marque les neuronnes et affiche le resultat
    */
    mark_neurons(neurons, neu_sizeX, neu_sizeY, normalized_vec_data, vec_size, vec_att_size, index);
    show_result(neurons, neu_sizeX, neu_sizeY);
    
    return 0;
}
