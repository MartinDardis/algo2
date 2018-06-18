#define _POSIX_C_SOURCE 200809L
#define LINE_SIZE 200
#include "tp2.h"
#include "heap.h"
#include "lista.h"
#include "abb.h"
#include "hash.h"


typedef struct log{
    char ip[20];
    char fecha[30];
    char metodo[5];
    char url[145];
}log_t;

typedef struct adhoc{
    size_t file_num;
    log_t* data;
}adhoc_t;
/************************* FUNCIONES DE COMPARACION***************************/
int line_cmp (const void* a,const void*b){
    int resultado;
    resultado = strcmp(((log_t*)a)->fecha , ((log_t*)b)->fecha);
    if (resultado == 0){
        resultado = strcmp(((log_t*)a)->ip , ((log_t*)b)->ip);
        if(resultado == 0){
            resultado = strcmp(((log_t*)a)->metodo , ((log_t*)b)->metodo);
        }
    }
    return resultado;
}

int heap_cmp (const void* a,const void*b){
    log_t* left = ((adhoc_t*)a)->data;
    log_t* right = ((adhoc_t*)b)->data;
    int resultado;
    resultado = strcmp(((log_t*)left)->fecha , ((log_t*)right)->fecha);
    if (resultado == 0){
        resultado = strcmp(((log_t*)left)->ip , ((log_t*)right)->ip);
        if(resultado == 0){
            resultado = strcmp(((log_t*)left)->metodo , ((log_t*)right)->metodo);
        }
    }
    if(resultado == 1) resultado = -1;
    else if (resultado == -1 ) resultado = 1;
    return resultado;
}
/*****************************************************************************/
/************************* Primitivas internas *******************************/
bool divide_and_sort(FILE* input,size_t max_lines,size_t* parts);
log_t** read_lines(FILE* input,size_t max_lines,size_t* read_lines);
bool save_lines(log_t** lines,size_t part_file_num,size_t top);
FILE* create_part_file(size_t part);
void free_lines(log_t**lines,size_t top);
bool merge_files(FILE* output,size_t num_parts);
bool open_part_files(FILE* files [],size_t num_parts);
bool load_heap(heap_t* out_heap,FILE* files[],size_t file_to_read);
bool write_out(heap_t* out_heap,FILE* output,FILE* files[],size_t num_parts);
/*****************************************************************************/
bool ordenar_archivo(const char* input_file,const char* output_file,const size_t max_memory){
    if(!input_file || !output_file) return false;
    FILE* input = fopen(input_file,"rt");
    FILE* output = fopen(output_file,"wt");
    size_t lines_in_parts = (max_memory*1000)/LINE_SIZE;
    bool error = false;
    size_t num_parts = 0;
    error = divide_and_sort(input,lines_in_parts,&num_parts);
    error = merge_files(output,num_parts);//tendrias que revisar si error es true antes de llamar esto y pisar el valor
    system("rm *.part");
    fclose(input);
    fclose(output);
    return !error;
}

bool divide_and_sort(FILE* input,size_t max_lines,size_t* parts){
    while(!feof(input)){
        size_t total_read_lines = 0;
        log_t** lines = read_lines(input,max_lines,&total_read_lines);
        if(!lines) return false;
        heap_sort((void**)lines,total_read_lines,line_cmp);
        bool save = save_lines(lines,*parts,total_read_lines);
        if(!save) return false;
        free_lines(lines,total_read_lines);
        (*parts)++;
    }
    return true;
}

log_t** read_lines(FILE* input,size_t max_lines,size_t* read_lines){
    log_t** logs = malloc(sizeof(log_t*) * max_lines);
    char* buffer = NULL;
    size_t cant = 0;
    size_t i;
    for(i = 0; i < max_lines && !feof(input);i++){
        logs[i] = malloc(sizeof(log_t));
        //fscanf(input,"%s\t%s\t%s\t%s",logs[i]->ip,logs[i]->fecha,logs[i]->metodo,logs[i]->url);
        if (getline(&buffer,&cant,input) > 0){
            char** line = split(buffer,'\t');
            strcpy(logs[i]->ip,line[0]);
            strcpy(logs[i]->fecha,line[1]);
            strcpy(logs[i]->metodo,line[2]);
            strcpy(logs[i]->url,line[3]);
            free_strv(line);
            free(buffer);
            buffer = NULL;
            cant = 0;
        }
        else i--;
    }
    (*read_lines) = i;
    return logs;
}

bool save_lines(log_t** lines,size_t part_file_num,size_t top){
    FILE* part_file = create_part_file(part_file_num);
    if(!part_file) return false;
    for(size_t i=0;i<top;i++){
        fwrite(lines[i],sizeof(log_t),1,part_file);
    }
    fclose(part_file);
    return true;
}

FILE* create_part_file(size_t part){
    char file_name[30];
    sprintf(file_name,"%i.part",(int)part);
    FILE* file = fopen(file_name,"w");
    return file;
}

void free_lines(log_t**lines,size_t top){
    for(size_t i = 0; i<top;i++){
        free(lines[i]);
    }
    free(lines);
}

bool merge_files(FILE* output,size_t num_parts){
    FILE* files [num_parts];
    if(!open_part_files(files,num_parts))return false;
    heap_t* out_heap = heap_crear(heap_cmp);
    for(size_t i=0; i < num_parts;i++){
        if(!load_heap(out_heap,files,i)) return false;
    }
    bool estate = write_out(out_heap,output,files,num_parts);
    heap_destruir(out_heap,NULL);
    for(size_t i = 0; i<num_parts; i++){
        fclose(files[i]);
    }
    return estate;
}

bool open_part_files(FILE* files [],size_t num_parts){
    for(size_t i=0 ; i<num_parts;i++){
        char file_name[30];
        sprintf(file_name,"%i.part",(int)i);
        files[i] = fopen(file_name,"r");
        if (!files[i]) return false;
    }
    return true;
}

bool load_heap(heap_t* out_heap,FILE* files[],size_t file_to_read){
    if(feof(files[file_to_read])) return true;
    adhoc_t* queue = malloc(sizeof(adhoc_t));
    if(!queue) return false;
    log_t* data = malloc(sizeof(log_t));
    if(!data) {
        free(queue);
        return false;
    }
    fread(data,sizeof(log_t),1,files[file_to_read]);
    queue->file_num = file_to_read;
    queue->data = data;
    heap_encolar(out_heap,queue);
    return true;
}

bool write_out(heap_t* out_heap,FILE* output,FILE* files[],size_t num_parts){
    while (! heap_esta_vacio(out_heap)) {
        adhoc_t* to_save = heap_desencolar(out_heap);
        log_t* data_to_save = to_save->data;
        fprintf(output, "%s\t",data_to_save->ip);
        fprintf(output, "%s\t",data_to_save->fecha);
        fprintf(output, "%s\t",data_to_save->metodo);
        fprintf(output, "%s\n",data_to_save->url);
        free(data_to_save);
        free(to_save);
        if(!load_heap(out_heap,files,to_save->file_num)) return false;
    }
    return true;
}



bool agregar_archivo(const char* file){
    if(!file) {
        return false;
    }
    FILE* input = fopen(file,"rt");
    hash_t* hash = hash_crear(lista_destruir);
    if(!hash){
        return false;
    }

    char* buffer = NULL;
    size_t cant = 0;
    char** line;

    char* ip;
    char* fecha;
    lista_t* lista;

    while (getline(&buffer,&cant,input) > 0){
        line = split(buffer,'\t');
        ip = line[0];
        fecha = line[1];

        lista = hash_obtener(hash, ip);
        if(!lista){
            lista = lista_crear();
            lista_insertar_ultimo(lista, fecha);
            hash_guardar(hash, ip, lista);
        } else {
            lista_insertar_ultimo(lista, fecha);
        }
        hash_guardar(hash, ip, fecha);
        free_strv(line);
    }
    free(buffer);
    fclose(input);

    hash_iter_t* hash_iter = hash_iter_crear(hash);
    if(!hash_iter){
        return false;
    }
    lista_iter_t* lista_iter_1;
    lista_iter_t* lista_iter_2;
    char* fecha_1;
    char* fecha_2;
    bool ip_actual_ataco;

    heap_t* heap = heap_crear(ip_cmp);
    if(!heap){
        return false;
    }

    while(!hash_iter_al_final(iter)){
        ip = hash_iter_ver_actual(iter);
        ip_actual_ataco = false;
        lista = hash_obtener(ip);

        lista_iter_1 = lista_iter_crear(lista);
        lista_iter_2 = lista_iter_crear(lista);
        for (int i = 0; i < 5; i++){
            lista_iter_avanzar(lista_iter_1);
        }

        while(!lista_iter_al_final(lista_1) && !ip_actual_ataco){
            if(time_diference(fecha_1; fecha_2) >= DOS_MAXIMUM_TIME){
                heap_encolar(heap, ip);
                ip_actual_ataco = true;
            }
        }

        lista_iter_destruir(lista_iter_1);
        lista_iter_destruir(lista_iter_2);

    }
    while(!heap_esta_vacio(heap)){
        printf("DoS: %s\n", heap_desencolar);
    }

    hash_iter_destruir(hash_iter);
    hash_destruir(hash);

    return true;
}

bool print_ip(const char * clave, void * dato, void * extra){
    printf(clave);
    return true;
}

bool ver_visitantes(const char* desde, const char* hasta){
    if(!desde || !hasta){
        return false;
    }
    abb_t* abb = abb_crear(ip_cmp, NULL);
    if(!abb){
        return false;
    }
    char* buffer = NULL;
    size_t cant = 0;
    char** line;

    char* ip;

    while (getline(&buffer,&cant,input) > 0){
        line = split(buffer,'\t');
        ip = line[0];
        abb_guardar(abb, ip, NULL);
    }
    free_strv(line);
    free(buffer);

    abb_in_order_desde_hasta(abb, print_ip, NULL, desde, hasta);
    abb_destruir(abb);
    return true;
}