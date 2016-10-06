#include "MALLOC.H"
#include "DOS.H"
#include "CONIO.H"
#include "STDIO.H"
#include "math.h"
#include "stdlib.h" 
#include "string.h"

struct WaveData {
  unsigned int SoundLength;
  unsigned int numSamples;
  unsigned short *Sample;
};

void cargarWAVE (struct WaveData *Voice, char *FileName);
void corregirHeader ( );
void escribirData ( );
void escribirWAVE (struct WaveData *Voice, char *FileName);

struct HeaderType {
  int            RIFF;              //RIFF header
  char           NI1 [18];          //No Importa
  unsigned short Canales;           //canales 1 = mono; 2 = stereo
  int            Frecuencia;        //frecuencia
  int            TasaBit;           //Tasa de bits Frecuencia * Canales * bitRes/8
  short          AlineamientoBloque;//Alineamento de los boques
  unsigned short BitRes;            //bit resolucion 8/16/32 bit
  int			 algo;				//No Importa
  int			 subChuckSize;		// NumSamples * NumChannels * BitsPerSample/8
} Header;

struct WaveData VoiceEntrada;
struct WaveData VoiceSalida1;
struct WaveData VoiceSalida2;

int main (int argc, char* argv[])
{
	int cantBitsMuestra;
	if (argc != 4)
	{
		printf ("Faltan argumentos\n");
		system("pause");
		return -1;
	}
	printf ("Direccion de Origen: %s\n",argv[1]);	
	printf ("Direccion de Escritura1: %s\n",argv[2]);
	printf ("Direccion de Escritura2: %s\n",argv[3]);
	printf ("Presione cualquier tecla para continuar.\n");



	printf ( "Ingrese la cantidad de bits por muestra en la pista ->");
	scanf( "%d", &cantBitsMuestra);

	system("pause");
	
	cargarWAVE(&VoiceEntrada, argv[1]);

	VoiceSalida1.SoundLength = floor(VoiceEntrada.SoundLength/2.0);
	VoiceSalida2.SoundLength = floor(VoiceEntrada.SoundLength/2.0);

	VoiceSalida1.Sample = (unsigned short * ) malloc (VoiceSalida1.SoundLength);
	VoiceSalida2.Sample = (unsigned short * ) malloc (VoiceSalida2.SoundLength);

	corregirHeader( );
	escribirData( cantBitsMuestra );

	escribirWAVE(&VoiceSalida1,argv[2]);
	escribirWAVE(&VoiceSalida2,argv[3]);
	system("pause");

}

void corregirHeader ( ) 
{
	Header.Canales = 1;
	Header.TasaBit = Header.Frecuencia * Header.BitRes / 8 * Header.Canales;
	Header.AlineamientoBloque = Header.BitRes / 8 * Header.Canales;
	Header.subChuckSize = Header.subChuckSize/2;
}

void escribirData ( int cantBits )
{
	int i;
	int posEntrada = 0;
	int posSalida1 = 0;
	int posSalida2 = 0;
	short a;
	int sh = 16-cantBits;
	short neg=-1<<sh;

	VoiceSalida1.Sample[0] = neg;
	posSalida1++; posEntrada++;
	VoiceSalida2.Sample[0] = neg;
	posSalida2++; posEntrada++;

	for (i = 1; i<VoiceEntrada.numSamples; i++)
	{
		a = VoiceEntrada.Sample[posEntrada];
		VoiceSalida1.Sample[posSalida1] = a&neg;
		posSalida1++; posEntrada++;
		a = VoiceEntrada.Sample[posEntrada];
		VoiceSalida2.Sample[posSalida2] = a&neg;
		posSalida2++; posEntrada++;
	}
}

void cargarWAVE (struct WaveData *Voice, char *FileName)
{
  FILE *WAVFile;

  WAVFile = fopen(FileName, "rb");

  if (WAVFile == NULL) {
    printf("No se logro leer el archivo");
    exit (0);
  }

     //Cargar el encabezado
  fread(&Header, 44, 1, WAVFile);

 
  Voice->SoundLength = Header.subChuckSize;
  free(Voice->Sample);
  Voice->Sample = (unsigned short *)malloc(Voice->SoundLength); //Asignar memoria
  if (!Voice->Sample) {
    exit (0);
  }

  //Check RIFF header
  if (Header.RIFF != 0x46464952) {
    printf ("Not a wave file");
    exit (0);
  }


  //Check canales
  if (Header.Canales != 2) {
    printf ("Not a stereo wave file");
    exit (0);
  }


  //Check resolución bits
  if (Header.BitRes != 16) {
    printf ("Not an 16-bit wave file");
    exit (0);
  }

  //Carga los muestreos
  fread(Voice->Sample, Voice->SoundLength, 1, WAVFile);
  
  fclose (WAVFile); 

  Voice -> numSamples = Header.subChuckSize / 4;

}

void escribirWAVE (struct WaveData *Voice, char *FileName)
{
  FILE *WAVFile;

  WAVFile = fopen(FileName, "wb");

  if (WAVFile == NULL) {
    printf("No se logro escribir el archivo");
    exit (0);
  }

  fwrite(&Header,44,1,WAVFile);
  fwrite(Voice->Sample, Voice->SoundLength, 1, WAVFile);

}
