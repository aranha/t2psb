#include <math.h>
#include <string.h>		// para usar strings

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Rotinas para leitura de arquivos .hdr
#include "rgbe.h"

// Variáveis globais a serem utilizadas:

// Dimensões da imagem de entrada
int sizeX, sizeY;

// Imagem de entrada
RGBf* image;

// Imagem de saída
RGB8* image8;

char* image_filename;

// Fator de exposição inicial
float exposure = 1.0;

// Image - heigth and width
int heigth, width;

//RGB by image

float r=0.0;
float g=0.0;
float b=0.0;
// Modo de exibição atual
int modo;

// Função pow mais eficiente (cerca de 7x mais rápida)
float fastpow(float a, float b);
float fastpow(float a, float b)
{
    union
    {
        float f;
        int i;
    }
    u = { a };
    u.i = (int)(b * (u.i - 1065307417) + 1065307417);
    return u.f;
}

// Função principal de processamento: ela deve chamar outras funções
// quando for necessário (ex: algoritmos de tone mapping, etc)
void process()
{

    for(int i=0; i<width*heigth; i++)
    {
        if (modo==0)
        {
            //Tone Mapping by scale
            r=(image[i].r/(image[i].r + 0.1)*exposure);
            g=(image[i].g/(image[i].g + 0.1)*exposure);
            b=(image[i].b/(image[i].b + 0.1)*exposure);
        }
        else
        {
            //Tone Mapping by gama correction
            r=((fastpow(image[i].r,(1.0/2.2)))*exposure);
            g=((fastpow(image[i].g,(1.0/2.2)))*exposure);
            b=((fastpow(image[i].b,(1.0/2.2)))*exposure);
        }

        //Conversion to 24 bits
        image8[i].r = (unsigned char) (fmin(1.0, r)*255);
        image8[i].g = (unsigned char) (fmin(1.0, g)*255);
        image8[i].b = (unsigned char) (fmin(1.0, b)*255);

    }
    printf("Exposure: %.3f\n", exposure);
    buildTex();
}

void readImage()
{
// Abre o arquivo
    FILE* arq = fopen(image_filename,"rb");

// Lê o header do arquivo, de onde são extraídas a largura e altura
    RGBE_ReadHeader(arq, &width, &heigth, NULL);

// TESTE: cria uma imagem de 800x600
    sizeX = width;
    sizeY = heigth;

    printf("%d x %d\n", sizeX, sizeY);

    // Aloca imagem float
    image = (RGBf *)malloc(sizeof(RGBf) * sizeX * sizeY);

    // Aloca memória para imagem de 24 bits
    image8 = (RGB8*) malloc(sizeof(RGB8) * sizeX * sizeY);

    // Aloca memória para a imagem inteira
    image = (RGBf*) malloc(sizeof(RGBf) * width * heigth);

// Finalmente, lê a imagem para a memória
    int result = RGBE_ReadPixels_RLE(arq, (float*)image, width, heigth);
    if (result == RGBE_RETURN_FAILURE)
    {
        /// Tratamento de erro...
        printf("ERRO!\n");
    }
    fclose(arq);
}

int main(int argc, char** argv)
{
    image_filename = "memorial.hdr";

    if(argc==1)
    {
        printf("hdrvis [image file.hdr]\n");
        exit(1);
    }

    // Inicialização da janela gráfica
    init(argc,argv);

    exposure = 1.0f; // exposição inicial

    readImage();

    // Aplica processamento inicial
    process();

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}

