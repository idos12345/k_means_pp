#define PY_SSIZE_T_CLEAN  /* For all # variants of unit formats (s#, y#, etc.) use Py_ssize_t rather than int. */
      /* MUST include <Python.h>, this implies inclusion of the following standard headers:       <stdio.h>, <string.h>, <errno.h>, <limits.h>, <assert.h> and <stdlib.h> (if available). */
#include <Python.h>
#include <math.h>         /* include <Python.h> has to be before any standard headers are included */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct node
{
    double* xi;
    struct node* next;
    struct node* prev;
} node;
typedef struct mu{
    double* mui;
    node* xi_list;
} mu;

typedef struct change{
    node* xi_node;
    int old_mu_index;
    int new_mu_index;
} change;

#define DEF_MAX_ITER 200
#define EPSILON 0.001

/* function decleration*/
double** read_data_from_file(FILE* fp, int number_of_cord, int number_of_lines);
double** read_all_lines_to_X(int number_of_lines, FILE* fp, int number_of_cord );
double* read_line_into_xi(int number_of_cord, char* curr_number_start, double* xi );
mu* initialze_mus_array(double** X, int K, int number_of_cord);
int argmin(double* xi, mu* mus, int K, int number_of_cords);
void add_x_to_mu(node* xi_node, mu* mui);
void delete_x_from_mu(node* xi_node, mu* mui);
void swap(node* xi_node, int old_mu, int new_mu, mu* mus);
double euqlide_norm(double* old_mu, double* new_mu, int number_of_cords);
double update_mus(mu* mus, int K, int number_of_cords);
int write_to_outputfile(mu* mus, int K, FILE* fp_out, int number_of_cords);
int compute_number_of_x(FILE *fp );
int initial_xi_liked_list( int number_of_lines, int K, int number_of_cords, mu* mus, double** X);
change* update_changes_array(mu* mus, change* change_array, int number_of_cords, int K );
void implementing_changes(mu* mus, int number_of_lines, change* change_array);
int K_mean(int K, int max_iter, double epsilon, char* data_filename, char* mus_filename);
void free_memory(double** X,double** Y, mu* mus, int num_of_X, int k);
int submit_args(int argc, char **argv, FILE** fp_in, FILE** fp_out, double* k, double* max_iter);
static PyObject* fit(PyObject *self, PyObject *args);


/* create an array of pointers to xi calld X
 output: read file and update  all_x_array -  an array of vertex , initial all mus to null*/
double** read_data_from_file(FILE* fp, int number_of_cord, int number_of_lines){
     /*double** X = (double**)calloc(number_of_lines , sizeof(double*));
     int line_number, i;
     double* xi;
     char* line;
     char* curr_number_start;
     char* curr_comma;*/
     rewind(fp);
        /*line = (char*)calloc(1024,sizeof(char));
        if (line == NULL){
            printf("An Error Has Occurred\n");
            return NULL;
        }
        for(line_number =0 ; line_number < number_of_lines; line_number++){
            curr_number_start = line;
            fscanf(fp, "%s",line);
            xi = (double*) calloc(number_of_cord,sizeof(double));
            if (xi == NULL){
                printf("An Error Has Occurred\n");
                return NULL; }
            for ( i =0; i<number_of_cord -1; i++){
                curr_comma = strchr((char*)curr_number_start,','); 
                *curr_comma = '\0'; 
                xi[i] = atof(curr_number_start); 
                curr_number_start = ++curr_comma; 
            }
            xi[number_of_cord-1] = atof(curr_number_start);
            X[line_number]= read_line_into_xi(number_of_cord,curr_number_start,xi); 
        }
        free(line);
        fclose(fp);*/
        return read_all_lines_to_X(number_of_lines, fp,number_of_cord);
}

/*read all lines into X*/
double** read_all_lines_to_X(int number_of_lines, FILE* fp, int number_of_cord ){
    int line_number;
    char* curr_number_start, line;
    double* xi;
    double** X = (double**)calloc(number_of_lines , sizeof(double*));
    line = (char*)calloc(1024,sizeof(char));
    if (line == NULL || X == NULL){
        printf("An Error Has Occurred\n");
        return NULL;
    }
    for(line_number =0 ; line_number < number_of_lines; line_number++){
        curr_number_start = line;
        fscanf(fp, "%s",line);
        xi = (double*) calloc(number_of_cord,sizeof(double));
        if (xi == NULL){
            printf("An Error Has Occurred\n");
            return NULL;
        }
        X[line_number]= read_line_into_xi(number_of_cord,curr_number_start,xi); 
    }
    free(line);
    fclose(fp);
    return X;
}

/* read line from file and write it to xi */
double* read_line_into_xi(int number_of_cord, char* curr_number_start, double* xi ){
    char* curr_comma;
    int word_num;
    for ( word_num =0; word_num<number_of_cord -1; word_num++){
        curr_comma = strchr((char*)curr_number_start,','); /* point to the first , in line */
        *curr_comma = '\0'; 
        xi[word_num] = atof(curr_number_start); /* insert word to xi */
        curr_number_start = ++curr_comma; /* update line to tne next char after , */
    }
    xi[number_of_cord-1] = atof(curr_number_start);
    return xi;
}
/* free memory at the end of k_means */
void free_memory(double** X,double** Y, mu* mus, int num_of_X, int k){
    node* curr;
    node* step;
    int i;
    for (i = 0; i < num_of_X; i++){
        free(X[i]);
    }
    free(X);

    for (i = 0; i < k; i++){
        free(Y[i]);
    }
    free(Y);

    for (i = 0; i < k; i++){
        free(mus[i].mui);
        curr = mus[i].xi_list;
        while(curr){
            step = curr->next;
            free(curr);
            curr = step;
        }
    }
    free((void*)mus);
}

/* create mu_array from first K xi from X
 output: return mu_array - the firt K xi from X */ 
mu* initialze_mus_array(double** X, int K, int number_of_cord){
    mu* mus = (mu*)calloc(K,sizeof(mu));
    int i, word;
    for(i =0; i< K; i++){
        mus[i].mui= (double*)calloc(number_of_cord,sizeof(double));
        if(mus[i].mui == NULL){
            printf("An Error Has Occurred\n");
            return NULL;
        }
        mus[i].xi_list = NULL;
        for(word =0; word < number_of_cord; word++){
            mus[i].mui[word] = X[i][word];
        }
    }
    return mus;
}


/* outpot: the index of its clostest mu of xi*/
int argmin(double* xi, mu* mus, int K, int number_of_cords){
   double min_sum, sum;
   int min_index=0, mu_index, cord;

    for(mu_index =0; mu_index < K; mu_index++ ){
       sum =0;
       for(cord = 0; cord< number_of_cords; cord++){
           sum += pow((xi[cord] - mus[mu_index].mui[cord]),2);
       }
       if (mu_index ==0) min_sum = sum;
        else if(sum<min_sum) {
            min_sum = sum;
            min_index = mu_index;
        } 
    }
    return min_index;
}

/* output: add node of xi to xi_list of mui*/
void add_x_to_mu(node* xi_node, mu* mui){
    xi_node->prev = NULL;
    if(mui->xi_list == NULL){ /* xi_list empty*/
        mui->xi_list = xi_node; 
        xi_node->next = NULL;
    }else{
    mui->xi_list->prev = xi_node;
    xi_node->next = mui->xi_list;
    mui->xi_list = xi_node;
    }
}

/* output: remove node ox xi frox xi_list of mui*/
void delete_x_from_mu(node* xi_node, mu* mui){
    if(xi_node->prev == NULL){ /* first node in list*/
        if(xi_node->next == NULL){ /* xi_node is the only node in list*/
            mui->xi_list = NULL;
        }else{
            xi_node->next->prev = NULL;
            mui->xi_list= xi_node->next;
        }
    } 
    else if (xi_node->next ==NULL){ /* last node in list*/
        xi_node->prev->next=  NULL;
    }
    else{ 
        xi_node->prev->next = xi_node->next;
        xi_node->next->prev = xi_node->prev;
    }
}

/* output: remote xi_node from old_mu list and add it to the start of new_mu list*/
void swap(node* xi_node, int old_mu, int new_mu, mu* mus){
    delete_x_from_mu(xi_node, &mus[old_mu]);
    add_x_to_mu(xi_node,&mus[new_mu]);
}

/*euqlide_norm
output: for every cord (sum of (old_mu[cord]**2 -new_mu[cord])**2 )**0.5*/
double euqlide_norm(double* old_mu, double* new_mu, int number_of_cords){
    double sum =0, power;
    int i;
    for(i =0; i< number_of_cords ; i++){
        power = pow(old_mu[i] - new_mu[i],2);
        sum += power;
    }
    sum = pow(sum,0.5);
    return sum;
}

/* output: compute and update new_mus_array and returns delta max */
double update_mus(mu* mus, int K, int number_of_cords){
    double deltamax =0, delta;
    int mu_index, xi_list_len, cord;
    double* new_mu;
    double* old_mu;
    node* curr_xi;

    for(mu_index =0; mu_index< K; mu_index++){
        xi_list_len =0;
        new_mu = (double*)calloc(number_of_cords, sizeof(double));
        if(new_mu == NULL){
            printf("An Error Has Occurred\n");
            return -1;
        }
        curr_xi = mus[mu_index].xi_list;
        while (curr_xi != NULL)
        {
            for(cord =0; cord<number_of_cords; cord++){
                new_mu[cord] += curr_xi->xi[cord];
            }
            xi_list_len++;
            curr_xi = curr_xi->next;
        }
        /* we assume xi_list_len != 0 becuse mui dosent have an empty list as noted in the forum */
        for(cord =0; cord< number_of_cords; cord++){
            new_mu[cord] = new_mu[cord]/xi_list_len;
        }
        old_mu =  mus[mu_index].mui;
        delta = euqlide_norm(old_mu, new_mu, number_of_cords);
        if (delta > deltamax) deltamax= delta;
        free(old_mu);
        mus[mu_index].mui = new_mu;
    }
    return deltamax;
}

/* output: create output.txt and write mus_array in it*/
int write_to_outputfile(mu* mus, int K, FILE* fp_out, int number_of_cords ){
    int mu, cord;
    for(mu =0; mu <K; mu++){
        for(cord =0; cord< number_of_cords; cord++){
             fprintf(fp_out,"%.4f",mus[mu].mui[cord]);
            if (cord == number_of_cords -1 && mu != K-1 ){
                fprintf(fp_out,"%s","\n");
            }else if (cord != number_of_cords-1){
                fprintf(fp_out,"%s",",");
            }     
        }
    }
    fprintf(fp_out,"%s","\n");
    fclose(fp_out);
    return 0; 
}

/*output: returns number of lines*/
int compute_number_of_x(FILE *fp ){
    char ch;
    int xi_counter =0;
    rewind(fp); /* goes to the beginning of th file*/
     do{
        ch = fgetc(fp);
        if(ch =='\n') xi_counter ++;
    } while (ch != EOF);
    return xi_counter ;
}
/* output: return the number of the coordinates in every x*/
int compute_number_of_cord(FILE *fp){
    char ch = '0';
    int cords_counter = 1;
    rewind(fp); /* goes to the beginning of th file*/
    while (ch!='\n'){
        ch = fgetc(fp);
        if(ch == ',') cords_counter++;
    }
  return cords_counter;
}
/*create node for each xi and insert it to it's closest mu linked list */
int initial_xi_liked_list( int number_of_lines, int K, int number_of_cords, mu* mus, double** X){
    int xi, new_mu;
    node* new_xi_node;
    for(xi = 0; xi< number_of_lines; xi++){
        new_mu = argmin(X[xi],mus,K,number_of_cords);
        new_xi_node = (node*)malloc(sizeof(node));
        if(new_xi_node == NULL ) {
            printf("An Error Has Occurred\n");
            return 0;
        }
        new_xi_node->xi = X[xi];
        add_x_to_mu(new_xi_node, &mus[new_mu]);
    }
    return 1;
}

/* for evry xi save its old mu & new mu in changes array */ 
change* update_changes_array(mu* mus, change* change_array, int number_of_cords, int K ){
    int new_mu, mu_index, xi_counter =0;
    node* curr_xi;
    for(mu_index =0; mu_index< K; mu_index++){
        curr_xi = mus[mu_index].xi_list;
        while (curr_xi != NULL){
            new_mu = argmin(curr_xi->xi,mus,K,number_of_cords);
            change_array[xi_counter].new_mu_index = new_mu;
            change_array[xi_counter].old_mu_index=mu_index;
            change_array[xi_counter].xi_node = curr_xi;
            xi_counter++;
            curr_xi = curr_xi->next;
        }
    }
    return change_array;
}

/* update mu's linked list according changes array */ 
void implementing_changes(mu* mus, int number_of_lines, change* change_array){
    int change_num,new_mu,old_mu;
    for(change_num = 0; change_num< number_of_lines; change_num++){
        new_mu = change_array[change_num].new_mu_index;
        old_mu = change_array[change_num].old_mu_index;
        if(new_mu != old_mu ){
            swap(change_array[change_num].xi_node, old_mu,new_mu, mus);
        }
    }
}


int K_mean(int K, int max_iter, double epsilon, char* data_filename, char* mus_filename){

    double maxdelta = epsilon;
    int  iter =0, number_of_cords, number_of_lines, memory_alocate;
    change* change_array;
    double** X; 
    mu* mus;
    FILE *data_file, *mus_file;

    data_file = fopen(data_filename,"r");
    mus_file = fopen(mus_filename,"r");

    number_of_cords = compute_number_of_cord(data_file);
    number_of_lines = compute_number_of_x(data_file);

    if(K > number_of_lines){
        printf("Invalid Input!\n");
        return 1;
    }
    X = read_data_from_file(data_file,number_of_cords,number_of_lines);
    if(X == NULL){
        printf("An Error Has Occurred\n");
        return 1;
    }

    double **Y = read_data_from_file(mus_file,number_of_cords,K);
    mus = initialze_mus_array(Y,K,number_of_cords);
    if(mus == NULL){
        printf("An Error Has Occurred\n");
        return 1;
    }

    memory_alocate= initial_xi_liked_list(number_of_lines, K, number_of_cords, mus,X);
    if(memory_alocate == 0) {
        printf("An Error Has Occurred\n");
        return 1;
    }

    fclose(mus_file);
    fclose(data_file);

    while (iter < max_iter && maxdelta >= epsilon){
        change_array =(change*) calloc(number_of_lines,sizeof(change));
        if(change_array == NULL ) {
            printf("An Error Has Occurred\n");
            return 1;
        }
        change_array = update_changes_array(mus,change_array,number_of_cords,K); /* update changes_array according new mus */
        implementing_changes(mus, number_of_lines,change_array); /* link evrey xi to its new mu according to changes_array */ 
        maxdelta = update_mus(mus,K,number_of_cords); /* compute new max delts */ 
        if(maxdelta == -1) {
            printf("An Error Has Occurred\n");
            return 1;
        }
        iter++;
        /*free change array*/
        free(change_array);   
    }

    mus_file = fopen(mus_filename,"w");
    write_to_outputfile(mus,K,mus_file,number_of_cords);
    fclose(mus_file);
    free_memory(X,Y,mus,number_of_lines,K);
    return 0;
}

/* submit args to vars, return 1 if successed else 0 */
int submit_args(int argc, char **argv, FILE** fp_in, FILE** fp_out, double* k, double* max_iter){
    char* input_file;
    char* output_file;
    char* eptr;
    if (argc != 4 && argc != 5){
        printf("Invalid Input!\n");
        return 1;
    }

    *k = strtod(argv[1], &eptr);

    /* if max_iter is not given */
    if (argc == 4){
        *max_iter = DEF_MAX_ITER;
        input_file = argv[2];
        output_file = argv[3];
    }

    /* if max_iter is given */
    if (argc == 5){
        *max_iter = strtod(argv[2], &eptr);
        input_file = argv[3];
        output_file = argv[4];
    }

    *fp_in  = fopen(input_file,"r");
    *fp_out = fopen(output_file,"w");
    /* one of the file didnt opened */
    if(fp_in == NULL || fp_out == NULL){
        printf("An Error Has Occurred\n");
        return 1;
    }
    /* input check */
    if (*k <= 0|| *k != (int)*k || *max_iter <= 0|| *max_iter != (int)*max_iter || *fp_in == NULL || fp_out == NULL){
        printf("Invalid Input!\n");
        return 1;
    }
    return 1;
}

int main(int argc, char **argv){ 
    /*double K_double, max_iter_double ;
    int K, max_iter;
    FILE* fp_in;
    FILE* mus;
    FILE* fp_out;
    if(submit_args(argc,argv,&fp_in,&fp_out,&K_double,&max_iter_double) == 0) return 1;
    K = (int) K_double;
    max_iter = (int)max_iter_double;
   K_mean(K,max_iter,fp_in, fp_out);*/
    return 0;
}

static PyObject* fit(PyObject *self, PyObject *args){
    int k,max_iter,epsilon;
    char *data_filename, *mus_filename;
    /* This parses the Python arguments into a double (d)  variable named z and int (i) variable named n*/
    if(!PyArg_ParseTuple(args, "iidss", &k,&max_iter,&epsilon,&data_filename, &mus_filename)) {
        printf("An Error Has Occurred\n");
        return NULL; /* In the CPython API, a NULL value is never valid for a
                        PyObject* so it is used to signal that an error has occurred. */
    }
    return Py_BuildValue("i", K_mean(k,max_iter,epsilon,data_filename,mus_filename)); /*  Py_BuildValue(...) returns a PyObject*  */
}

static PyMethodDef capiMethods[] = {
    {"k_means",                   /* the Python method name that will be used */
      (PyCFunction) fit, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,           /* flags indicating parametersaccepted for this function */
      PyDoc_STR("kmeans ++")}, /*  The docstring for the function */
    {NULL, NULL, 0, NULL}     /* The last entry must be all NULL as shown to act as a
                                 sentinel. Python looks for this entry to know that all
                                 of the functions for the module have been defined. */
};

/* This initiates the module using the above definitions. */
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    capiMethods /* the PyMethodDef array from before containing the methods of the extension */
};

/*
 * The PyModuleDef structure, in turn, must be passed to the interpreter in the module’s initialization function.
 * The initialization function must be named PyInit_name(), where name is the name of the module and should match
 * what we wrote in struct PyModuleDef.
 * This should be the only non-static item defined in the module file
 */
PyMODINIT_FUNC
PyInit_mykmeanssp(void){
    PyObject *m;
    m = PyModule_Create(&moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}
