#include <stdio.h>//Libreria estandar, manejo de archivos
#include <stdlib.h>// Numeros aleatorios
#include <omp.h>// Para el paralelismo
#include <math.h>//Formulas
#include <time.h> //Tiempo para los numeros aleatorios

#define STB_IMAGE_IMPLEMENTATION //de código abierto que realiza todo el manejo de este tipo de archivos, para solo trabajar con los arreglos de datos (pixeles)
#include "stb-master/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"

#define M 256 //Los valores que puede tomar

int numProcesadores, ancho, alto, canales,i;
double tiempoFinalSecuencial,tiempoInicialSecuencial,tiempoTotalSecuencial=0, tiempoFinalParalelo,
 tiempoInicialParalelo,tiempoTotalParalelo=0, speedUp=0, tiempoOverHead=0,eficiencia=0,
 tIniCargaSecu,tFinCargaSecu,tIniCargaParalel,tFinCargaParalel,tIniGenera,tFinGenera,tIniSecuCSV,
 tFinSecuCSV,tIniParalelCSV,tFinParalelCSV;
char *usuarioImagen;
char jpg[] = ".jpg";
char nombreImagenEntrada[23];
char nombreImagenSalida[23];
char nombreImagenSalidaParalelo[23];
void ecualizadorImagenes(int numeroProcesadores);

int main(int args[], char *argv[]){
    srand(time(NULL));
    printf("-----------Estructura de datos y algortimos II-----------\n\n");
    printf("\nIntegrantes:\n-Mondrag%cn Hern%cndez Andrea Quetzalli.\n-Montes Cantero Zelene Yosseline Isayana.\n",162,160);
    printf("\nBienvenido al ecualizador de imagenes\n");
    usuarioImagen = argv[1];
    //Creando nombre de entrada de la imagen
    strcat(nombreImagenEntrada,usuarioImagen);
    strcat(nombreImagenEntrada,jpg);

    //Numero de hilos igual al numero de procesadores
    numProcesadores = omp_get_num_procs();

    //Llamo a la funcion
    ecualizadorImagenes(numProcesadores);
    system ("PAUSE");
    system ("cls");
}

//Funcion ecualizar imagenes
void ecualizadorImagenes(int numProcesadores){
    omp_set_num_threads(numProcesadores);
    int histogramaSecuencial[M], histogramaParalelo[M],CDF[M], CDFParalelo[M], eqCdf[M], eqCdfParalelo[M], histo2P[M], histo2[M];
    char *imagenj = nombreImagenEntrada;

    tIniGenera = omp_get_wtime();
    //Cargar la imagen
    unsigned char *loadImage = stbi_load(imagenj, &ancho, &alto, &canales, 0);//Tipo de variable para el manejo de pixeles (el valor de un pixel es de 8 bites)
    tFinGenera = omp_get_wtime();

    if(loadImage == NULL){
        printf("\nNo se pudo cargar la imagen %s:", imagenj);
        return;
    }else{
        printf("\n**********Imagen cargada exitosamente**********\n\n");
    }

    if(canales != 1 && canales !=3)
        printf("\nERROR. Solo se aceptan imagenes de 1 y 3 canales");

    if(canales == 1){
        //Ecualizar imagenes de forma Secuencial
        //printf("\nLa imagen es de %dx%d pixeles de tamanio %d con %d canale(s)",ancho,alto,ancho*alto,canales);

    tiempoInicialSecuencial = omp_get_wtime();
    //Llenar de ceros el arreglo del histograma Secuencial y el CDF
    for(int i=0; i<M; i++){
        histogramaSecuencial[i] = 0;//Para ver las ocurrencias de los pixeles
        CDF[i] = 0;
    }

    //Se ven las ocurrencias
     for(int i=0; i<ancho*alto; i++)
        histogramaSecuencial[loadImage[i]]++;

    //Haciendo la función de distribución acumulativa (CDF)
    CDF[0] = histogramaSecuencial[0];
    for(int i=1; i<M; i++)
        CDF[i] = histogramaSecuencial[i]+CDF[i-1];

   // Funcion para buscar el maximo secuencial
    int CDFmaximo = CDF[0];
    for (int i=1; i<M; i++)// Hasta M que es 256
        if(CDFmaximo < CDF[i])
            CDFmaximo = CDF[i];
    //printf("\nEl maximo es %d", CDFmaximo);

    // Funcion para buscar el minimo secuencial
    int CDFminimo = CDFmaximo;//Se toma de referencia el maximo para encontrar el minimo
    for (int i=0; i<M; i++)// Hasta M que es 256
        if(CDFminimo > CDF[i] && CDF[i]!=0)//Minimo debe de ser deferente de 0
            CDFminimo = CDF[i];
    //printf("\nEl minimo es %d", CDFminimo);

    //Hacer mi arreglo eqCdf ya esta ecualizada
    for(int i=0; i<M; i++)
        //Es necesario castear el cociente a float para que no se pierdan los decimales
        eqCdf[i] = round(((float)(CDF[i]-CDFminimo)/(float)((ancho*alto)-CDFminimo))*(M-2))+1;//Formula para equalizar

    //Genero el arreglo de pixeles de la nueva imagen
    unsigned char *eqImage = malloc(ancho*alto);//de tamaño width*height de un canal, se crea espacio en memoria
    for (int i=0; i<ancho*alto; i++)
        eqImage[i] = eqCdf[loadImage[i]];//Nuevos valores de los pixeles ya ecualizados de la imagen

    for(int i=0; i<M; i++){
        histo2[i] = 0;//Para ver las ocurrencias de los pixeles
    }
    for (int i=0; i<ancho*alto; i++)
        histo2[eqImage[i]]++;

    tiempoFinalSecuencial = omp_get_wtime();

    //Creando nombre de entrada de la imagen
    int casilla;
    char nombreAleaotio [20][20]={"_EDA", "_Andrea", "_Zelene","_Jesus","_Tazz", "_imagen","_1","_2","_3","_4","_5","_6","_7","_8","_9","_10","_11","_12"};//Arreglo bidimensional
    casilla = rand()%17;
    char *nombreArchivo = nombreAleaotio[casilla];
    strcat(nombreImagenSalida,"secu_");//prefijo
    strcat(nombreImagenSalida,usuarioImagen);//nombre de la imagen que ingreso el usuario
    strcat(nombreImagenSalida, nombreArchivo);// nombre aleatorio de el arreglo bidimensional
    strcat(nombreImagenSalida,jpg);// formato de la imagen
    char *imagenSalida = nombreImagenSalida;

    //Crear la nueva imagen
    tIniCargaSecu = omp_get_wtime();
    stbi_write_jpg(imagenSalida, ancho, alto, canales, eqImage,100); //porcentaje de calidad que debe ser 100 (para no compresión).
    //Libero la memoria, de los arreglos dinámicos:
    stbi_image_free(eqImage);
    tFinCargaSecu = omp_get_wtime();

    tIniSecuCSV = omp_get_wtime();
  //Crear el CSV para poder guardar los datos
    FILE *flujo = fopen("output/histograma_secuencial.csv", "w"); //Si no existe lo crea en el momento a difernecia de "a" de append, variable de tipo FILE
    if(flujo == NULL) printf("Error en la creacion del archivo\n\n"); //Imprime el error
    else{
            for(int i=0; i<M; i++)
                fprintf(flujo,"%d, %d, %d\n", histogramaSecuencial[i],CDF[i],histo2[i]);//Se le ponen comas , esto hara que se separen por columnas, y el salto de linea para el renglon
    }

    fflush(flujo);//Limpiando el buffer
    fclose(flujo);//Cerrando el flujo de datos
    tFinSecuCSV = omp_get_wtime();

    tiempoTotalSecuencial = tiempoFinalSecuencial-tiempoInicialSecuencial;


//--------------------------------------------------------------------------------------------
    //Ecualizar imagenes de forma Paralela
    int CDFmaximoParalelo = CDFParalelo[0];//Se inicia con el 0 para ver el maximo
    int CDFminimoParalelo;
    unsigned char *eqImageParalelo = malloc(ancho*alto);//de tamaño width*height de un canal

    tiempoInicialParalelo = omp_get_wtime();
    #pragma omp parallel //Se abre un bloque en paralelo
    {
    #pragma omp for //es tomar el valor de una variable aportada por cada hilo y aplicar la operación indicada sobre esos para obtener un resultado
    //Llenar de ceros el arreglo del histograma Paralelo y el CDF
    for(i=0; i<M; i++){
        histogramaParalelo[i] = 0;
        CDFParalelo[i] = 0;
    }
    #pragma omp barrier //Todos los hijos esperan

    #pragma omp for reduction(+:histogramaParalelo)
    //Se ven las ocurrencias
     for(i=0; i<ancho*alto; i++)
        histogramaParalelo[loadImage[i]]++;

    //Haciendo la función de distribución acumulativa (CDF)
    # pragma omp single //Solo un hilo lo realiza , aquí todos los hilos esperan
    {
    CDFParalelo[0] = histogramaParalelo[0];
    for(i=1; i<M; i++)
        CDFParalelo[i] = histogramaParalelo[i] + CDFParalelo[i-1];
    }

    // Funcion para buscar el maximo secuencial
    // Hasta M que es 256
    #pragma omp for reduction(max:CDFmaximoParalelo)
    for (i=1; i<M; i++){
        if(CDFmaximoParalelo < CDFParalelo[i]){
            CDFmaximoParalelo = CDFParalelo[i];
        }
    }

    // Funcion para buscar el minimo secuencial
    CDFminimoParalelo = CDFmaximoParalelo;
    #pragma omp parallel for reduction(min:CDFminimoParalelo)
    for (i=0; i<M; i++)// Hasta M que es 256
        if(CDFminimoParalelo > CDFParalelo[i] && CDFParalelo[i]!=0)
            CDFminimoParalelo = CDFParalelo[i];

    //Hacer mi arreglo eqCdf ya esta ecualizada
    #pragma omp  for
    for(i=0; i<M; i++)
         eqCdfParalelo[i] = round(((float)(CDFParalelo[i]-CDFminimoParalelo)/(float)((ancho*alto)-CDFminimoParalelo))*(M-2))+1;

    //Genero el arreglo de pixeles de la nueva imagen
    #pragma omp  for
    for (i=0; i<ancho*alto; i++)
        eqImageParalelo[i] = eqCdfParalelo[loadImage[i]];


    #pragma omp  for
    for(int i=0; i<M; i++){
        histo2P[i] = 0;//Para ver las ocurrencias de los pixeles
    }
    #pragma omp barrier

    #pragma omp for reduction(+:histo2P)
    for (int i=0; i<ancho*alto; i++)
         histo2P[eqImageParalelo[i]]++;

    }//Se termina el bloque de codigo en paralelo

    tiempoFinalParalelo = omp_get_wtime();

    //Creando nombre de entrada de la imagen
    strcat(nombreImagenSalidaParalelo,"paralelo_");//Sufijo
    strcat(nombreImagenSalidaParalelo,usuarioImagen);//ingresada por el usuario
    strcat(nombreImagenSalidaParalelo, nombreArchivo);//casilla aleatoria
    strcat(nombreImagenSalidaParalelo,jpg);
    char *imagenSalidaParalelo = nombreImagenSalidaParalelo;

    tIniCargaParalel = omp_get_wtime();
    //Crear la nueva imagen
    stbi_write_jpg(imagenSalidaParalelo, ancho, alto, canales, eqImageParalelo, 100);
    //Libero la memoria, de los arreglos dinámicos:
    stbi_image_free(eqImageParalelo);
    tFinCargaParalel = omp_get_wtime();

    tIniParalelCSV = omp_get_wtime();

     //Crear el CSV para poder guardar los datos
        FILE *flujoParalelo = fopen("output/histograma_paralelo.csv", "w"); //Si no existe lo crea en el momento a difernecia de "a", de append
        if(flujoParalelo == NULL) printf("Error en la creacion del archivo\n\n"); //Imprime el error//Variable tipo FILE
        else{
                for(int i=0; i<M; i++)
                    fprintf(flujoParalelo,"%d, %d, %d\n", histogramaParalelo[i],CDFParalelo[i],histo2P[i]);
        }

        fflush(flujoParalelo);//Limpiando el buffer
        fclose(flujoParalelo);//Cerrando el flujo de datos
        tFinParalelCSV = omp_get_wtime();

        tiempoTotalParalelo = tiempoFinalParalelo-tiempoInicialParalelo;

        //Metricas
        speedUp = tiempoTotalSecuencial/tiempoTotalParalelo; // mejora la versión serial a la de paralelo
        eficiencia = speedUp/numProcesadores; // Esta dependerá del número de procesadores. Si da mayor a uno, se está midiendo mal
        tiempoOverHead = tiempoTotalParalelo-(tiempoTotalSecuencial/numProcesadores);// Este se refiere al tiempo “perdido” de un programa en paralelo


        printf("\nOTRAS METRICAS");
        printf("\n\tTiempo de generacion de imagen: %f segundos", tFinCargaSecu-tIniCargaSecu );

        printf("\n\tS e c u e n c i a l");
        printf("\n\tEl nombre de la imagen ecualizada es: %s", nombreImagenSalida);
        printf("\n\t\tTiempo de carga de imagen : %f segundos", tFinGenera-tIniGenera);
        printf("\n\t\tTiempo de generacion de archivos CSV ", tFinSecuCSV-tIniSecuCSV);

        printf("\n\tP a r a l e l o");
        printf("\n\tEl nombre de la imagen ecualizada es: %s", nombreImagenSalidaParalelo);
        printf("\n\t\tTiempo de carga de imagen Paralelo: %f segundos", tFinCargaParalel-tIniCargaParalel);
        printf("\n\t\tTiempo de generacion de archivos CSV Paralelo: %f segundos", tFinParalelCSV-tIniParalelCSV);

        printf("\n\n T I E M P O S");
        printf("\n\tTiempo Secuencial: %f segundos", tiempoTotalSecuencial);
        printf("\n\tTiempo Paralelo: %f segundos", tiempoTotalParalelo);

        printf("\nM E T R I C A S");
        printf("\n\tTamanio de la imagen: %d", ancho*alto);
        printf("\n\tAncho: %d ",ancho);
        printf("\n\tAlto: %d ",alto);
        printf("\n\t%d canales",canales);
        printf("\n\tSpeedUp: %.16f", speedUp);
        printf("\n\tEl numero de procesadores: %d", numProcesadores);
        printf("\n\tEficiencia: %.16f", eficiencia);
        printf("\n\tTiempo OverHead: %f", tiempoOverHead);

        }

//--------------------------------------------------------------------------------------------

    if(canales == 3){
        unsigned char *imagen3canals = malloc(ancho*alto*canales);//crea espacio en memoria

        tiempoInicialSecuencial = omp_get_wtime();
        for(int i=0; i<ancho*alto; i++){
            //Se obtienes los valores de los pixeles de los colores
            unsigned char r = loadImage[i*canales];//Color rojo, asi es solicitado en el proyecto
            imagen3canals[i*canales] = r;
        }

        //Llenar de ceros el arreglo del histograma Secuencial y CDF Secuencial
        for(int i=0; i<M; i++){
            histogramaSecuencial[i] = 0;//Para ver las ocurrencias
            CDF[i] = 0;
        }

        //Se ven las ocurrencias
         for(int i=0; i<ancho*alto; i++)
            histogramaSecuencial[imagen3canals[i]]++;

        //Haciendo la función de distribución acumulativa (CDF)
        CDF[0] = histogramaSecuencial[0];
        for(int i=1; i<M; i++)
            CDF[i] = histogramaSecuencial[i]+CDF[i-1];

       // Funcion para buscar el maximo secuencial
        int CDFmaximo = CDF[0];//Tomando de referencia el 0
        for (int i=1; i<M; i++)// Hasta M que es 256
            if(CDFmaximo < CDF[i])
                CDFmaximo = CDF[i];
       // printf("\nEl maximo es %d", CDFmaximo);

        // Funcion para buscar el minimo secuencial
        int CDFminimo = CDFmaximo;
        for (int i=0; i<M; i++)// Hasta M que es 256
            if(CDFminimo > CDF[i] && CDF[i]!=0)
                CDFminimo = CDF[i];
      //  printf("\nEl minimo es %d", CDFminimo);

        //Hacer mi arreglo eqCdf ya esta ecualizada
        for(int i=0; i<M; i++)
            eqCdf[i] = round(((float)(CDF[i]-CDFminimo)/(float)((ancho*alto)-CDFminimo))*(M-2))+1;//Pixeles ya acualizados

        //Genero el arreglo de pixeles de la nueva imagen
        unsigned char *eqImage = malloc(ancho*alto*canales);//de tamaño width*height de un canal, 3 veces mas grande por ser de 3 canales
        for (int i=0; i<ancho*alto*canales; i++)
            eqImage[i] = eqCdf[loadImage[i]];


        for(int i=0; i<M; i++){
            histo2[i] = 0;//Para ver las ocurrencias de los pixeles
        }
        for (int i=0; i<ancho*alto; i++)
            histo2[eqImage[i]]++;


        tiempoFinalSecuencial = omp_get_wtime();

        //Creando nombre de entrada de la imagen
        int casilla;
        char nombreAleaotio [20][20]={"_EDA", "_Andrea", "_Zelene","_Jesus","_Tazz", "_imagen","_1","_2","_3","_4","_5","_6","_7","_8","_9","_10","_11","_12"};
        casilla = rand()%17;
        char *nombreArchivo = nombreAleaotio[casilla];
        strcat(nombreImagenSalida,"secuencialRBG_");
        strcat(nombreImagenSalida,usuarioImagen);
        strcat(nombreImagenSalida, nombreArchivo);
        strcat(nombreImagenSalida,jpg);
        char *imagenSalida = nombreImagenSalida;

        tIniCargaSecu = omp_get_wtime();
        //Crear la nueva imagen
        stbi_write_jpg(imagenSalida, ancho, alto, canales, eqImage,100);
        //Libero la memoria, de los arreglos dinámicos:
        stbi_image_free(eqImage);
        tFinCargaSecu = omp_get_wtime();

        tIniSecuCSV = omp_get_wtime();
        //Crear el CSV para poder guardar los datos
        FILE *flujoRGB = fopen("output/histograma_secuencialRGB.csv", "w"); //Si no existe lo crea en el momento a difernecia de "a"
        if(flujoRGB == NULL) printf("Error en la creacion del archivo\n\n"); //Imprime el error
        else{
                for(int i=0; i<M; i++)
                    fprintf(flujoRGB,"%d, %d, %d\n", histogramaSecuencial[i],CDF[i],histo2[i]);
        }

        fflush(flujoRGB);//Limpiando el buffer
        fclose(flujoRGB);//Cerrando el flujo de datos
        tFinSecuCSV = omp_get_wtime();

        tiempoTotalSecuencial= tiempoFinalSecuencial-tiempoInicialSecuencial;


//--------------------------------------------------------------------------------------------
        //Ecualizar imagenes de forma Paralela
        unsigned char *imagen3canalsParalelo = malloc(ancho*alto*canales);
        unsigned char *eqImageParalelo = malloc(ancho*alto*canales);//de tamaño width*height de un canal

        int CDFmaximoParalelo = CDFParalelo[0];
        int CDFminimoParalelo;
        tiempoInicialParalelo= omp_get_wtime();
        #pragma omp parallel
        {

        #pragma omp single
        {
            for(i=0; i<ancho*alto; i++){
                //Se obtienes los valores de los pixeles de los colores
                unsigned char r = loadImage[i*canales];//Color rojo
                imagen3canalsParalelo[i*canales] = r;
            }
       }

        #pragma omp for
        //Llenar de ceros el arreglo del histograma Secuencial y el CDF
        for(i=0; i<M; i++){
            histogramaParalelo[i] = 0;
            CDFParalelo[i] = 0;
        }
        #pragma omp barrier

        #pragma omp for reduction(+:histogramaParalelo)
        //Se ven las ocurrencias
         for(i=0; i<ancho*alto; i++)
            histogramaParalelo[imagen3canalsParalelo[i]]++;

        //Haciendo la función de distribución acumulativa (CDF)
        #pragma omp single
        {
        CDFParalelo[0] = histogramaParalelo[0];
        for(i=1; i<M; i++)
            CDFParalelo[i] = histogramaParalelo[i] + CDFParalelo[i-1];
        }

       // Funcion para buscar el maximo secuencial
       // Hasta M que es 256
        #pragma omp for reduction(max:CDFmaximoParalelo)
        for (i=1; i<M; i++){
            if(CDFmaximoParalelo < CDFParalelo[i]){
                CDFmaximoParalelo = CDFParalelo[i];
            }
        }

        CDFminimoParalelo = CDFmaximoParalelo;
        #pragma omp parallel for reduction(min:CDFminimoParalelo)
        for (i=0; i<M; i++)// Hasta M que es 256
            if(CDFminimoParalelo > CDFParalelo[i] && CDFParalelo[i]!=0)
                CDFminimoParalelo = CDFParalelo[i];

       //Hacer mi arreglo eqCdf ya esta ecualizada
        #pragma omp for
        for(i=0; i<M; i++)
            eqCdfParalelo[i] = round(((float)(CDFParalelo[i]-CDFminimoParalelo)/(float)((ancho*alto)-CDFminimoParalelo))*(M-2))+1;

        #pragma omp for
        for (i=0; i<ancho*alto*canales; i++)
            eqImageParalelo[i] = eqCdfParalelo[loadImage[i]];

        #pragma omp  for
        for(int i=0; i<M; i++){
            histo2P[i] = 0;//Para ver las ocurrencias de los pixeles
        }
        #pragma omp barrier

        #pragma omp for reduction(+:histo2P)
        for (int i=0; i<ancho*alto; i++)
            histo2P[eqImageParalelo[i]]++;
        }
        tiempoFinalParalelo = omp_get_wtime();

        //Creando nombre de entrada de la imagen
        strcat(nombreImagenSalidaParalelo,"paraleloRBG_");
        strcat(nombreImagenSalidaParalelo,usuarioImagen);
        strcat(nombreImagenSalidaParalelo, nombreArchivo);
        strcat(nombreImagenSalidaParalelo,jpg);
        char *imagenSalidaParalelo = nombreImagenSalidaParalelo;

        tIniCargaParalel = omp_get_wtime();
        //Crear la nueva imagen
        stbi_write_jpg(imagenSalidaParalelo, ancho, alto, canales, eqImageParalelo,100);
        //Libero la memoria, de los arreglos dinámicos:
        stbi_image_free(eqImageParalelo);
        tFinCargaParalel = omp_get_wtime();

        tIniParalelCSV = omp_get_wtime();
        //Crear el CSV para poder guardar los datos
        FILE *flujoParalelo = fopen("output/histograma_paraleloRGB.csv", "w"); //Si no existe lo crea en el momento a difernecia de "a"
        if(flujoParalelo == NULL) printf("Error en la creacion del archivo\n\n"); //Imprime el error
        else{
                for(int i=0; i<M; i++)
                    fprintf(flujoParalelo,"%d, %d, %d\n", histogramaParalelo[i],CDFParalelo[i],histo2P[i]);
        }

        fflush(flujoParalelo);//Limpiando el buffer
        fclose(flujoParalelo);//Cerrando el flujo de datos
        tFinParalelCSV = omp_get_wtime();

        tiempoTotalParalelo= tiempoFinalParalelo-tiempoInicialParalelo;
        //--------------------------------------------------------------------------------------------

        //Metricas
        speedUp = tiempoTotalSecuencial/tiempoTotalParalelo;
        eficiencia = speedUp/numProcesadores;
        tiempoOverHead = tiempoTotalParalelo-(tiempoTotalSecuencial/numProcesadores);

        printf("\nOTRAS METRICAS");
        printf("\n\tTiempo de generacion de imagen: %f segundos", tFinCargaSecu-tIniCargaSecu );

        printf("\n\tS e c u e n c i a l");
        printf("\n\tEl nombre de la imagen ecualizada es: %s", nombreImagenSalida);
        printf("\n\t\tTiempo de carga de imagen : %f segundos", tFinGenera-tIniGenera);
        printf("\n\t\tTiempo de generacion de archivos CSV ", tFinSecuCSV-tIniSecuCSV);

        printf("\n\tP a r a l e l o");
        printf("\n\tEl nombre de la imagen ecualizada es: %s", nombreImagenSalidaParalelo);
        printf("\n\t\tTiempo de carga de imagen Paralelo: %f segundos", tFinCargaParalel-tIniCargaParalel);
        printf("\n\t\tTiempo de generacion de archivos CSV Paralelo: %f segundos", tFinParalelCSV-tIniParalelCSV);

        printf("\n\n T I E M P O S");
        printf("\n\tTiempo Secuencial: %f segundos", tiempoTotalSecuencial);
        printf("\n\tTiempo Paralelo: %f segundos", tiempoTotalParalelo);

        printf("\nM E T R I C A S");
        printf("\n\tTamanio de la imagen: %d", ancho*alto);
        printf("\n\tAncho: %d ",ancho);
        printf("\n\tAlto: %d ",alto);
        printf("\n\t%d canales",canales);
        printf("\n\tSpeedUp: %.16f", speedUp);
        printf("\n\tEl numero de procesadores: %d", numProcesadores);
        printf("\n\tEficiencia: %.16f", eficiencia);
        printf("\n\tTiempo OverHead: %f", tiempoOverHead);
      }
}
